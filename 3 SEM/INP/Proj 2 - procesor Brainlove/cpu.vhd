-- cpu.vhd: Simple 8-bit CPU (BrainLove interpreter)
-- Copyright (C) 2016 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): DOPLNIT
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru

   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti

   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni (1) / zapis (0)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti

   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data

   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

 -- zde dopiste potrebne deklarace signalu
----------------------------- Multiplexer select signal
signal MUX_SEL : std_logic_vector(1 downto 0);
----------------------------- TMP register
signal TMP_REG : std_logic_vector(7 downto 0);
signal TMP_LD : std_logic;
----------------------------- PC register
signal PC_REG : std_logic_vector(11 downto 0);
signal PC_INC : std_logic;
signal PC_DEC : std_logic;
----------------------------- Instructions
type instructions is (I_INC_PTR, I_DEC_PTR, I_INC_DATA, I_DEC_DATA, I_PUTC, I_GETC, I_WHILE_BE, I_WHILE_EN, I_STORE_TMP, I_LOAD_TMP, I_BREAK, I_HALT, I_IDLE);
signal IREG_CODE : instructions;
----------------------------- Pointer register
signal PTR_REG : std_logic_vector(9 downto 0);
signal PTR_INC : std_logic;
signal PTR_DEC : std_logic;
----------------------------- Counter register
signal CNT_REG : std_logic_vector(7 downto 0);
signal CNT_INC : std_logic;
signal CNT_DEC : std_logic;
signal CNT_LD : std_logic;

----------------------------- FSM States
type states is (S_INIT, S_GETC_1, S_GETC_2, S_GETC_3, S_INSTR, S_FETCH_0, S_INC_PTR, S_DEC_PTR, S_INC_DATA_1, S_INC_DATA_2, S_DEC_DATA_1, S_DEC_DATA_2, S_GETC, S_STORE_TMP, S_NULL_OP, S_IDLE, S_WRITE_1, S_WRITE_2, S_WHILE_BE_1, S_WHILE_BE_2, S_WHILE_BE_3, S_WHILE_BE_4, S_WHILE_EN_1, S_WHILE_EN_2, S_WHILE_EN_3, S_WHILE_EN_4, S_WHILE_EN_5, S_LOAD_TMP_1, S_LOAD_TMP_2);
signal CURR_FSM: states;
signal NEXT_FSM: states;

begin
CODE_ADDR <= PC_REG;
DATA_ADDR <= PTR_REG;
 -- zde dopiste vlastni VHDL kod

----------------------------- Instructions decoder
instrDecode: process(CODE_DATA)
begin
    case CODE_DATA is
        when X"3E" => IREG_CODE <= I_INC_PTR;       -- >
        when X"3C" => IREG_CODE <= I_DEC_PTR;       -- <
        when X"2B" => IREG_CODE <= I_INC_DATA;      -- +
        when X"2D" => IREG_CODE <= I_DEC_DATA;      -- -
        when X"5B" => IREG_CODE <= I_WHILE_BE;      -- [
        when X"5D" => IREG_CODE <= I_WHILE_EN;      -- ]
        when X"2E" => IREG_CODE <= I_PUTC;          -- .
        when X"2C" => IREG_CODE <= I_GETC;          -- ,
        when X"24" => IREG_CODE <= I_LOAD_TMP;      -- $
        when X"21" => IREG_CODE <= I_STORE_TMP;     -- !
        when X"00" => IREG_CODE <= I_HALT;       -- null
        when others => IREG_CODE <= I_IDLE;
    end case;
end process;


----------------------------- MEM ptr
ptr: process(EN, RESET, CLK, PTR_INC, PTR_REG)
begin
    if (RESET = '1') then
        PTR_REG <= "0000000000";
    elsif (CLK'event and CLK = '1'  and EN = '1') then
        if (PTR_INC = '1') then
            PTR_REG <= PTR_REG + 1;
        elsif (PTR_DEC = '1') then
            PTR_REG <= PTR_REG - 1;
        end if;
    end if;
end process;

DATA_ADDR <= PTR_REG;

----------------------------- PC counter
pc: process(EN, RESET, CLK, PC_INC, PC_DEC)
begin
    if (RESET = '1') then
        PC_REG <= "000000000000";
    elsif (CLK'event and CLK = '1' and EN = '1') then
        if (PC_INC = '1') then
            PC_REG <= PC_REG + 1;
        elsif (PC_DEC = '1') then
            PC_REG <= PC_REG - 1;
        end if;
    end if;
end process;

CODE_ADDR <= PC_REG;

----------------------------- Loop counter
cnt: process(EN, RESET, CLK)
begin
    if (RESET = '1') then
        CNT_REG <= "00000000";
    elsif (CLK'event and CLK = '1' and EN = '1') then
	if (CNT_LD = '1') then 
	    CNT_REG(7 downto 0) <= "00000001";
    	elsif (CNT_INC = '1') then
            CNT_REG <= CNT_REG + 1;
        elsif (CNT_DEC = '1') then
            CNT_REG <= CNT_REG - 1;
        end if;
    end if;
end process;

----------------------------- Temp register
tmp: process(RESET, CLK)
begin
    if (RESET = '1') then
	TMP_REG <= "00000000";
    elsif (CLK'event and CLK = '1') then
    	if (TMP_LD = '1') then
	        TMP_REG <= DATA_RDATA;
    	end if;
    end if;
end process;

----------------------------- Multiplexer
mux: process(RESET, CLK, MUX_SEL)
begin
    case MUX_SEL is
    	when "00" => DATA_WDATA <= IN_DATA;
    	when "01" => DATA_WDATA <= DATA_RDATA + 1;
    	when "10" => DATA_WDATA <= DATA_RDATA - 1;
    	when "11" => DATA_WDATA <= TMP_REG;
    	when others => null;
    end case;
end process;

----------------------------- Current state
fsm_curr: process(RESET, CLK)
begin
    if (RESET = '1') then
        CURR_FSM <= S_FETCH_0;
    elsif (CLK'event and CLK = '1') then
        if (EN = '1') then
            CURR_FSM <= NEXT_FSM;
        end if;
    end if;
end process;

----------------------------- Next state
fsm_next: process(IN_VLD, OUT_BUSY ,IREG_CODE, CURR_FSM)
begin

----------------------------- Init state
    PC_INC <= '0';
    PC_DEC <= '0';

    CODE_EN <= '0';
    DATA_RDWR <= '0';
    OUT_WE <= '0';
    DATA_EN <= '0';
    IN_REQ <= '0';

    PTR_INC <= '0';
    PTR_DEC <= '0';

    CNT_DEC <= '0';
    CNT_INC <= '0';
    CNT_LD <= '0';

    TMP_LD <= '0';


    case (CURR_FSM) is
        when S_IDLE =>
            NEXT_FSM <= S_FETCH_0;
	        PC_INC <= '1';
----------------------------- READ INSTRUCTION
        when S_FETCH_0 =>
            NEXT_FSM <= S_INSTR;
            CODE_EN <= '1';
----------------------------- CHOOSE NEXT STATE
        when S_INSTR =>
            case IREG_CODE is
                when I_INC_PTR =>
                    NEXT_FSM <= S_INC_PTR;
                when I_DEC_PTR =>
                    NEXT_FSM <= S_DEC_PTR;
                when I_INC_DATA =>
                    NEXT_FSM <= S_INC_DATA_1;
                when I_DEC_DATA =>
                    NEXT_FSM <= S_DEC_DATA_1;
                when I_WHILE_BE =>
                    NEXT_FSM <= S_WHILE_BE_1;
                when I_WHILE_EN =>
                    NEXT_FSM <= S_WHILE_EN_1;
                when I_PUTC =>
                    NEXT_FSM <= S_WRITE_1;
                when I_GETC =>
                    NEXT_FSM <= S_GETC_1;
                when I_IDLE =>
                    NEXT_FSM <= S_IDLE;
                when I_HALT =>
                    NEXT_FSM <= S_NULL_OP;
        		when I_STORE_TMP =>
        		    NEXT_FSM <= S_STORE_TMP;
        		when I_LOAD_TMP =>
        		    NEXT_FSM <= S_LOAD_TMP_1;
                when others =>
                    NEXT_FSM <= S_IDLE;
            end case;
----------------------------- INCREASE POINTER
        when S_INC_PTR =>
            NEXT_FSM <= S_FETCH_0;
            PC_INC <= '1';
            PTR_INC <= '1';

----------------------------- DECREASE POINTER
        when S_DEC_PTR =>
            NEXT_FSM <= S_FETCH_0;
            PTR_DEC <= '1';
	        PC_INC <= '1';

----------------------------- INCREASE VALUE
        when S_INC_DATA_1 =>
            NEXT_FSM <= S_INC_DATA_2;
            DATA_EN <= '1';
            DATA_RDWR <= '1';

        when S_INC_DATA_2 =>
            NEXT_FSM <= S_FETCH_0;
    	    MUX_SEL <= "01";
    	    DATA_EN <= '1';
    	    DATA_RDWR <= '0';
    	    PC_INC <= '1';

----------------------------- DECREASE VALUE
        when S_DEC_DATA_1 =>
            NEXT_FSM <= S_DEC_DATA_2;
            DATA_EN <= '1';
            DATA_RDWR <= '1';

        when S_DEC_DATA_2 =>
            NEXT_FSM <= S_FETCH_0;
    	    MUX_SEL <= "10";
    	    DATA_EN <= '1';
    	    DATA_RDWR <= '0';
    	    PC_INC <= '1';

----------------------------- PUTCHAR
    when S_WRITE_1 =>
	    NEXT_FSM <= S_WRITE_2;
	    DATA_EN <= '1';
	    DATA_RDWR <= '1';

	when S_WRITE_2 =>
	    if (OUT_BUSY = '0') then
    		OUT_WE <= '1';
    		OUT_DATA <= DATA_RDATA;
    		PC_INC <= '1';
    		NEXT_FSM <= S_FETCH_0;
	    else
		    NEXT_FSM <= S_WRITE_2;
	    end if;

----------------------------- WHILE BEGIN
    when S_WHILE_BE_1 =>
        NEXT_FSM <= S_WHILE_BE_2;
        PC_INC <= '1';
        DATA_EN <= '1';
        DATA_RDWR <= '1';

    when S_WHILE_BE_2 =>
        if(DATA_RDATA(7 downto 0) = "00000000") then
            NEXT_FSM <= S_WHILE_BE_3;
            CNT_LD <= '1';
        else
            NEXT_FSM <= S_FETCH_0;
        end if;

    when S_WHILE_BE_3 =>
        if (CNT_REG(7 downto 0) /= "00000000") then
            NEXT_FSM <= S_WHILE_BE_4;
            CODE_EN <= '1';
        else
            NEXT_FSM <= S_FETCH_0;
        end if;

    when S_WHILE_BE_4 =>
        NEXT_FSM <= S_WHILE_BE_3;
        PC_INC <= '1';
        case (IREG_CODE) is
            when I_WHILE_BE =>
                CNT_INC <= '1';
            when I_WHILE_EN =>
                CNT_DEC <= '1';
            when others =>
                null;
        end case;
----------------------------- WHILE ENDING
    when S_WHILE_EN_1 =>
        NEXT_FSM <= S_WHILE_EN_2;
        DATA_RDWR <= '1';
        DATA_EN <= '1';

    when S_WHILE_EN_2 =>
        if (DATA_RDATA(7 downto 0) = "00000000") then
            NEXT_FSM <= S_FETCH_0;
            PC_INC <= '1';
        else
            NEXT_FSM <= S_WHILE_EN_3;
            CNT_LD <= '1';
            PC_DEC <= '1';
        end if;

    when S_WHILE_EN_3 =>
        if (CNT_REG(7 downto 0) /= "00000000") then
            NEXT_FSM <= S_WHILE_EN_4;
            CODE_EN <= '1';
        else
            NEXT_FSM <= S_FETCH_0;
        end if;

    when S_WHILE_EN_4 =>
        NEXT_FSM <= S_WHILE_EN_5;
        case (IREG_CODE) is
            when I_WHILE_BE =>
                CNT_DEC <= '1';
            when I_WHILE_EN =>
                CNT_INC <= '1';
            when others =>
                null;
        end case;

    when S_WHILE_EN_5 =>
        NEXT_FSM <= S_WHILE_EN_3;
        if (CNT_REG(7 downto 0) /= "00000000") then
            PC_DEC <= '1';
        else
            PC_INC <= '1';
        end if;

----------------------------- GETCHAR
    when S_GETC_1 =>
    	NEXT_FSM <= S_GETC_2;
    	IN_REQ <= '1';
    when S_GETC_2 =>
    	if (IN_VLD = '1') then
    	    NEXT_FSM <= S_GETC_3;
    	    MUX_SEL <= "00";
    	    DATA_EN <= '1';
    	    DATA_RDWR <= '0';
    	else
    	    NEXT_FSM <= S_GETC_2;
    	end if;
    when S_GETC_3 =>
    	NEXT_FSM <= S_FETCH_0;
    	PC_INC <= '1';
----------------------------- LOAD TMP FROM MEMORY
    when S_LOAD_TMP_1 =>
        NEXT_FSM <= S_LOAD_TMP_2;
        DATA_EN <= '1';
        DATA_RDWR <= '1';

    when S_LOAD_TMP_2 =>
        NEXT_FSM <= S_FETCH_0;
        TMP_LD <= '1';
        PC_INC <= '1';

----------------------------- STORE TMP TO MEMORY
    when S_STORE_TMP =>
        NEXT_FSM <= S_FETCH_0;
        DATA_EN <= '1';
        DATA_RDWR <= '0';
        MUX_SEL <= "11";
        PC_INC <= '1';

----------------------------- NULL
    when S_NULL_OP =>
	    NEXT_FSM <= S_NULL_OP;

----------------------------- COMMENTARY
    when others =>
	   NEXT_FSM <= S_IDLE;
    end case;
end process;


end behavioral;
