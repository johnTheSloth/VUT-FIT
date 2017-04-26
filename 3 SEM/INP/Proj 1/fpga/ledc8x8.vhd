library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

entity ledc8x8 is
port ( -- Sem doplnte popis rozhrani obvodu.
    SMCLK: in std_logic;
    ROW: out std_logic_vector (0 to 7);
    LED: out std_logic_vector (0 to 7);
    RESET: in std_logic	
);
end ledc8x8;

architecture main of ledc8x8 is
	signal ce: std_logic;	--clock enable
	signal cre: std_logic := '0'; -- credentials
	signal xrow: std_logic_vector(7 downto 0);
    signal xled: std_logic_vector(7 downto 0);
	signal cnt: std_logic_vector(7 downto 0);
	signal clk_cnt: integer range 0 to 3686500 := 0; 

begin

    -- Sem doplnte popis funkce obvodu (zakladni konstrukce VHDL jako napr.
    -- prirazeni signalu, multiplexory, dekodery, procesy...).
    -- DODRZUJTE ZASADY PSANI SYNTETIZOVATELNEHO VHDL UVEDENE NA WEBU:
    -- http://merlin.fit.vutbr.cz/FITkit/docs/navody/synth_templates.html

    -- Nezapomente take doplnit mapovani signalu rozhrani na piny FPGA
    -- v souboru ledc8x8.ucf.

    switch: process(SMCLK, RESET)
	begin
		if RESET ='1' then	--reset cnt
			cnt <="00000000";
		elsif SMCLK'event and SMCLK = '1' then
			cnt <= cnt + 1;
			if cnt (7 downto 0) ="11111111" then --smclk/256 aktivuje se kdyz counter napocita do 255
				ce <='1';
			else
				ce <='0';
			end if;
		end if;
	end process;
	
	switch_cnt:process(SMCLK, RESET) -- rozhoduje delku zobrazeni inicialu
	begin
		if RESET ='1' then
			clk_cnt <=0; -- reset cnt
		elsif SMCLK ='1' and SMCLK'event then
            if (clk_cnt = 3686400) then --kdyz counter napocita do poloviny frekvence (znaci pul sekundu)
            	clk_cnt <= 0000000;		--resetuje se
		    	if (cre  = '1') then	--a prohodi hodnotu cre, ktere rozhoduje jaky inicial se zobrazuje
			   	cre <= '0';
		    	elsif (cre = '0') then
		        	cre <='1';
				end if;
        	else
        		clk_cnt <= 1 + clk_cnt; --inkrementace counteru
        	end if;	
		end if;
	end process;

	rotate: process(SMCLK,RESET,ce,xrow)
	begin
		if RESET = '1' then
			xrow <= "10000000"; -- reset na vrch displeje
		elsif (SMCLK = '1' and SMCLK'event and ce = '1') then
			xrow <= xrow(0) & xrow(7 downto 1);
		end if;
		ROW <= xrow(7 downto 0); --nastaveni radku pomoci xrow
	end process;
	
	view :process(xrow, cre)
	begin
		if cre = '0' then -- J
			case xrow is
				when "10000000" => xled <= "11111111";
				when "01000000" => xled <= "11111011";
				when "00100000" => xled <= "11111011";
				when "00010000" => xled <= "11111011";
				when "00001000" => xled <= "11011011";
				when "00000100" => xled <= "11011011";
				when "00000010" => xled <= "11100111";
				when "00000001" => xled <= "11111111";
				when others     => xled <= "11111111";
			end case;
		elsif cre = '1' then -- G
			case xrow is
				when "10000000" => xled <= "11111111";
				when "01000000" => xled <= "11100111";
				when "00100000" => xled <= "11011011";
				when "00010000" => xled <= "11011111";
				when "00001000" => xled <= "11010011";
				when "00000100" => xled <= "11011011";
				when "00000010" => xled <= "11100111";
				when "00000001" => xled <= "11111111";
				when others     => xled <= "11111111";
			end case;
		end if;
	end process;

	ROW <= xrow;
	LED <= xled;

end main;
