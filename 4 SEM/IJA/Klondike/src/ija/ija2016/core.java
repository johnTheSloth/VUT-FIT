package ija.ija2016;

import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.io.File;
import java.util.ArrayList;
import java.util.List;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;


/**
 * Core class of program
 * @author xgross09
 * @version 1.0
 */
public class core {
    static int gamesC = 1;
    /**
     * Core class that create main window and all buttons and also handles events.
     * @param args Program arguments
     */
    public static void main(String[] args) {
        // first create new frame
        JFrame frame = new JFrame();
        frame.setTitle("IJA - Klondike Project");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        
        // add top panel - menubar
        JMenuBar menuBar = new JMenuBar();
        frame.setJMenuBar(menuBar);
        
        // add option Window to menubar
        JMenu games = new JMenu("Window");
        JMenuItem playOne = new JMenuItem("One");
        JMenuItem playMultiple = new JMenuItem("Multiple");
        JMenuItem exit = new JMenuItem("Exit");
        menuBar.add(games);
        games.add(playOne);
        games.add(playMultiple);
        games.add(exit);
        // create save/load options
        JMenu saveSub1 = new JMenu("Load");
        saveSub1.setName("Load Game 1");
        JMenu saveSub2 = new JMenu("Load");
        saveSub2.setName("Load Game 2");
        JMenu saveSub3 = new JMenu("Load");
        saveSub3.setName("Load Game 3");
        JMenu saveSub4 = new JMenu("Load");
        saveSub4.setName("Load Game 4");
        // add save/load option for each game to top panel
        JMenu save1 = new JMenu("Game 1");
        JMenu save2 = new JMenu("Game 2");
        JMenu save3 = new JMenu("Game 3");
        JMenu save4 = new JMenu("Game 4");
        
        JMenuItem actualSave = new JMenuItem("Save Game 1");
        JMenuItem newG1 = new JMenuItem("New Game");
        JMenuItem endG1 = new JMenuItem("End Game");
        save1.add(newG1);
        save1.add(endG1);
        save1.add(actualSave);
        save1.add(saveSub1);
        menuBar.add(save1);
        
        JMenuItem actualSave2 = new JMenuItem("Save Game 2");
        JMenuItem newG2 = new JMenuItem("New Game");
        JMenuItem endG2 = new JMenuItem("End Game");
        save2.add(newG2);
        save2.add(endG2);
        save2.add(actualSave2);
        save2.add(saveSub2);
        menuBar.add(save2);
        
        JMenuItem actualSave3 = new JMenuItem("Save Game 3");
        JMenuItem newG3 = new JMenuItem("New Game");
        JMenuItem endG3 = new JMenuItem("End Game");
        save3.add(newG3);
        save3.add(endG3);
        save3.add(actualSave3);
        save3.add(saveSub3);
        menuBar.add(save3);
        
        JMenuItem actualSave4 = new JMenuItem("Save Game 4");
        JMenuItem newG4 = new JMenuItem("New Game");
        JMenuItem endG4 = new JMenuItem("End Game");
        save4.add(newG4);
        save4.add(endG4);
        save4.add(actualSave4);
        save4.add(saveSub4);
        menuBar.add(save4);
        
        // init 4 games, only canvas1 will be visible on start
        Gui canvas1 = new Gui(1000,800);
        Gui canvas2 = new Gui(700,600);
        Gui canvas3 = new Gui(700,600);
        Gui canvas4 = new Gui(700,600);
        
        // add canvas 1 to main frame
        frame.add(canvas1);
        canvas1.newGame();
        //canvas1.resize(1000,800);
        
        // Exit button clicked
        class exit implements ActionListener {
            @Override
            public void actionPerformed(ActionEvent e) {
                System.exit(0);
            }
        }
        
        // End Game 1 button clicked
        class eg1 implements ActionListener {
            @Override
            public void actionPerformed(ActionEvent e) {
                canvas1.setGame(false);
                canvas1.setFirstRun(true);
                canvas1.repaint();
            }
        }
        
        // End Game 2 button clicked
        class eg2 implements ActionListener {
            @Override
            public void actionPerformed(ActionEvent e) {
                canvas2.setGame(false);
                canvas2.setFirstRun(true);
                canvas2.repaint();
            }
        }
        
        // End Game 3 button clicked
        class eg3 implements ActionListener {
            @Override
            public void actionPerformed(ActionEvent e) {
                canvas3.setGame(false);
                canvas3.setFirstRun(true);
                canvas3.repaint();
            }
        }
        
        // End Game 4 button clicked
        class eg4 implements ActionListener {
            @Override
            public void actionPerformed(ActionEvent e) {
                canvas4.setGame(false);
                canvas4.setFirstRun(true);
                canvas4.repaint();
            }
        }
        
        // New Game 1 button clicked
        class ng1 implements ActionListener {
            @Override
            public void actionPerformed(ActionEvent e) {
                canvas1.newGame();
                System.out.println("CLICK Play Again");
            }
        }
        
        // New Game 2 button clicked
        class ng2 implements ActionListener {
            @Override
            public void actionPerformed(ActionEvent e) {
                canvas2.newGame();
                System.out.println("CLICK Play Again");
            }
        }
        
        // New Game 3 button clicked
        class ng3 implements ActionListener {
            @Override
            public void actionPerformed(ActionEvent e) {
                canvas3.newGame();
                System.out.println("CLICK Play Again");
            }
        }
        
        // New Game 4 button clicked
        class ng4 implements ActionListener {
            @Override
            public void actionPerformed(ActionEvent e) {
                canvas4.newGame();
                System.out.println("CLICK Play Again");
            }
        }
        
        
        // Load button clicked
        class loadClick implements ActionListener {
            @Override
            public void actionPerformed(ActionEvent e) {
                JMenuItem menuItem = (JMenuItem) e.getSource();
                
                System.out.println(" " +menuItem.getName());
                if(menuItem.getName().equals("Load Game 1")) {
                    canvas1.loadBoard("examples/saves/"+menuItem.getActionCommand());
                }
                if(menuItem.getName().equals("Load Game 2")) {
                    canvas2.loadBoard("examples/saves/"+menuItem.getActionCommand());
                }
                if("Load Game 3".equals(menuItem.getName())) {
                    canvas3.loadBoard("examples/saves/"+menuItem.getActionCommand());
                }
                if("Load Game 4".equals(menuItem.getName())) {
                    canvas4.loadBoard("examples/saves/"+menuItem.getActionCommand());
                }
            }
        }
        
        // save button clicked
        class saveClick implements ActionListener {
            @Override
            public void actionPerformed(ActionEvent e) {
                JMenuItem menuItem = (JMenuItem) e.getSource(); 
                
                if("Save Game 1".equals(menuItem.getText())) {
                    canvas1.saveBoard("G1");
                }
                if("Save Game 2".equals(menuItem.getText())) {
                    canvas2.saveBoard("G2");
                }
                if("Save Game 3".equals(menuItem.getText())) {
                    canvas3.saveBoard("G3");
                }
                if("Save Game 4".equals(menuItem.getText())) {
                    canvas4.saveBoard("G4");
                }
                // need to reload names of saved games
                reloadSaves(save1, saveSub1,new loadClick());
                reloadSaves(save2, saveSub2,new loadClick());
                reloadSaves(save3, saveSub3,new loadClick());
                reloadSaves(save4, saveSub4,new loadClick());
            }
        }
        
        // Handle switch to layout for only 1 game
        class layOne implements ActionListener {
            @Override
            public void actionPerformed(ActionEvent e) {
                core.gamesC=1;
                canvas1.resize(1000,800);
                frame.remove(canvas2);
                frame.remove(canvas3);
                frame.remove(canvas4);
                frame.setLayout(new GridLayout(1, 1));
                frame.pack();
            }
        }
        
        // Handle switch to 2 and more games
        class layMultiple implements ActionListener {
            @Override
            public void actionPerformed(ActionEvent e) {
                core.gamesC=2;
                frame.setLayout(new GridLayout(2, 2));
                canvas1.resize(700, 600);
                frame.add(canvas2);
                frame.add(canvas3);
                frame.add(canvas4);
                frame.pack();
            }
        }
        
        // Create load options for each game
        reloadSaves(save1, saveSub1,new loadClick());
        reloadSaves(save2, saveSub2,new loadClick());
        reloadSaves(save3, saveSub3,new loadClick());
        reloadSaves(save4, saveSub4,new loadClick());
        // Add listeners to buttons
        newG1.addActionListener(new ng1());
        newG2.addActionListener(new ng2());
        newG3.addActionListener(new ng3());
        newG4.addActionListener(new ng4());
        endG1.addActionListener(new eg1());
        endG2.addActionListener(new eg2());
        endG3.addActionListener(new eg3());
        endG4.addActionListener(new eg4());
        actualSave.addActionListener(new saveClick());
        actualSave2.addActionListener(new saveClick());
        actualSave3.addActionListener(new saveClick());
        actualSave4.addActionListener(new saveClick());
        playOne.addActionListener(new layOne());
        playMultiple.addActionListener(new layMultiple());
        exit.addActionListener(new exit());
        
        
        // frame settings
        frame.pack();
        // show frame
        frame.setVisible(true);
        
        // Manage resizing of windows
        frame.addComponentListener(new ComponentAdapter() {
            @Override
            public void componentResized(ComponentEvent e) {
                canvas1.resize((int)(frame.getWidth()/core.gamesC), (int)(frame.getHeight()/core.gamesC));
            }
        });
        
    }
    
    // this functions create clickable item for each save saved in saves folder

    /**
     *
     * @param save Jmenu
     * @param saveSub Jmenu
     * @param load ActionListener
     */
    static public void reloadSaves(JMenu save, JMenu saveSub, ActionListener load) {
        List<String> results = new ArrayList<>();

        File[] files = new File("examples/saves").listFiles();
        
        saveSub.removeAll();
        for (File file : files) {
            if (file.isFile()) {
                //System.out.println(file.getName());
                JMenuItem load1 = new JMenuItem(file.getName());
                load1.setName(saveSub.getName());
                saveSub.add(load1);
            }
        }
        save.add(saveSub);
        
        for (int i = 0; i < saveSub.getItemCount(); i++) {
            saveSub.getItem(i).addActionListener(load);
        }
    }
    
    
}
