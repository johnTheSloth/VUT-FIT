package ija.ija2016;

import ija.ija2016.model.board.Board;
import ija.ija2016.model.cards.Card;
import ija.ija2016.model.cards.Pile;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.sql.Timestamp;
import java.text.SimpleDateFormat;
import java.util.Date;
import javax.swing.JPanel;

/**
 * GUI class
 * @author xgross09
 */
public class Gui extends JPanel {
    Color green = Color.decode("0x136d15");
    Board board;
    
    int from= 0; //tempPack 5, pile 10+i, target 20+i
    int to = 0; //tempPack 5, pile 10+i, target 20+i
    int cardNum = 0;
    int x,y;
    int hsize;
    boolean game = false;
    boolean help = false;
    boolean firstRun = true;
    
    boolean highlight = false;
    int hx, hy;
    
    boolean follow = false;
    int fx, fy;

    /**
     * Basic setter
     *
     * @param game boolean value
     */
    public void setGame(boolean game) {
        this.game = game;
    }

    /**
     * Boolean setter
     * @param firstRun boolean value
     */
    public void setFirstRun(boolean firstRun) {
        this.firstRun = firstRun;
    }
    
    /**
     * Set game to init state, ready to play.
     */
    public void newGame() {
        this.setGame(true);
        this.setFirstRun(false);
        this.board = new Board();
        repaint();
    }
    
    /**
     * Computes values on window resize
     * @param WX width of frame
     * @param WY height of frame
     */
    @Override
    public void resize(int WX, int WY) {
        setBackground(Color.GREEN);
        setPreferredSize(new Dimension(WX,WY));
        Card.setWidth((int)(WX-80)/7);
        Card.setHeight((int)(WY-20)/4);
    }
    
    /**
     * Basic constructor
     * @param WX width of frame
     * @param WY height of frame
     */
    public Gui(int WX, int WY) {
        setBackground(Color.GREEN);
        //setSize(new Dimension(800,600));
        setPreferredSize(new Dimension(WX,WY));
        Card.setWidth((int)(WX-80)/7);
        Card.setHeight((int)(WY-20)/4);
        board = new Board();
        MouseAdapter ma = new MyMouseAdapter();
        addMouseListener(ma);
        addMouseMotionListener(ma);
    }
    
    /**
     * Main draw function
     * @param g Graphics
     */
    @Override
    public void paint(Graphics g) {
        // Pre-Game
        if(!game) {
            // redraw board
            g.setColor(green);
            g.fillRect(0,0,getWidth(), getHeight());
            // write pre-game text
            g.setColor(Color.BLACK);
            g.setFont(new Font("Tahoma", Font.BOLD, 49));
            if(firstRun) {
                // uncomment if neeeded
                //g.drawString("IJA Projekt: Klondike", getWidth()/2-290, getHeight()/2-100);
            }else {
                g.drawString("You won the game!!", getWidth()/2-270, getHeight()/2-100);
                
            }
            // draw play again rectangle
            /*
             * uncomment if needed 
            g.setColor(Color.LIGHT_GRAY);
            g.fillRect(getWidth()/2-150, getHeight()/2+50, 300, 50);
            g.setColor(Color.BLACK);
            g.fillRect(getWidth()/2-145, getHeight()/2+50, 290, 50);
            // write play again
            g.setColor(Color.LIGHT_GRAY);
            g.setFont(new Font("Tahoma", Font.BOLD, 35));
            g.drawString("> Start Game <", getWidth()/2-154, getHeight()/2+88);
            */
        }else {
        // Actual game
            // draw board
            g.setColor(green);
            g.fillRect(0,0,getWidth(), getHeight());
            
            // draw help
            java.awt.Color temp = new java.awt.Color(255,255,255,70);
            g.setColor(temp);
            g.fillRect(2*(Card.cardWidth + 10)+10, 10, Card.cardWidth, Card.cardHeight/2-5);
            g.setFont(new Font("Tahoma", Font.BOLD, (int)(Card.cardWidth*0.25)));
            g.setColor(Color.BLACK);
            g.drawString("HELP", 2*(Card.cardWidth + 10)+10+(int)(Card.cardWidth*0.17), 10+Card.cardHeight/2-(int)(Card.cardHeight*0.2));
            
            // draw UNDO
            
            g.setColor(temp);
            g.fillRect(2*(Card.cardWidth + 10)+10, 15+Card.cardHeight/2, Card.cardWidth, Card.cardHeight/2-5);
            g.setFont(new Font("Tahoma", Font.BOLD, (int)(Card.cardWidth*0.25)));
            g.setColor(Color.BLACK);
            g.drawString("UNDO", 2*(Card.cardWidth + 10)+10+(int)(Card.cardWidth*0.1), 30+Card.cardHeight/2+(int)(Card.cardHeight*0.2));
            
            //draw pack
            g.setColor(java.awt.Color.DARK_GRAY);
            g.fillRect(10, 10, Card.cardWidth,Card.cardHeight);
            g.setColor(new java.awt.Color(239, 207, 151));
            g.fillRect(10+1, 10+1, Card.cardWidth-1, Card.cardHeight-2);
            g.setColor(new java.awt.Color(140, 0, 12,40));
            g.fillRect(10+2, 10+2, Card.cardWidth-2, Card.cardHeight-3);
            g.setColor(new java.awt.Color(237, 150, 0,30));
            g.fillRect(10+Card.cardWidth-40, 10, 30, Card.cardHeight-5);
            

            //draw tempPack
            board.getPack().draw(g,10+Card.cardWidth+10,10);

            // draw piles
            int off = 0;
            for (int i = 0; i < 7; i++) {
                board.getPile(i).draw(g, 10,10+Card.cardHeight+10, off);
                off += Card.cardWidth+10;
            }

            // targets
            off = 0;
            for (int i = 0; i < 4; i++) {
                board.getTarget(i).draw(g, 3*Card.cardWidth+4*10, 10, off);
                off += Card.cardWidth+10;
            }
            
            // Draw highlight overlay on click
            if(highlight) {
                int height = (hsize-1-cardNum)*Pile.pPart + Card.cardHeight;
                temp = new java.awt.Color(0, 0, 0,200);
                g.setColor(temp);
                g.fillRect(hx, hy-height, Card.cardWidth, height);
            }
            
            // Drawing help overlay
            if(help) {
                // fromX, toX
                int fx =0, fy=0, tx=0, ty=0;
                int height = (hsize-1-cardNum)*Pile.pPart + Card.cardHeight;
                if(board.getHelpFrom() == 0) {
                    fx = 10;
                    fy = Card.cardHeight+10;
                    height = Card.cardHeight;
                }
                // Positions setting
                // From PACK
                if(board.getHelpFrom() == 5) {
                    // show from
                    fx = Card.cardWidth+20;
                    fy = Card.cardHeight+10;
                    // Only 1 card can be moved from pack
                    height = Card.cardHeight;
                    if(board.getHelpTo() >= 20) {
                        // Pack card straight to taraget
                        tx = 3*(Card.cardWidth+10)+10+(board.getHelpTo()-20)*(Card.cardWidth+10);
                        ty = Card.cardHeight+10;
                    }else if(board.getHelpTo() >= 10) {
                        // Pack card to pile
                        tx = 10+(board.getHelpTo()-10)*(Card.cardWidth+10);
                        ty = 2*(Card.cardHeight+10)+(board.getPile(board.getHelpTo()-10).size()*Pile.pPart); 
                    }
                }else if(board.getHelpFrom() >= 10 && board.getHelpFrom() <= 19) {
                // From PILE
                    // Pile card to target
                    if(board.getHelpTo() >= 20) {
                        height = Card.cardHeight;
                        fx = 10+(board.getHelpFrom()-10)*(Card.cardWidth+10);
                        fy = 2*(Card.cardHeight+10)+((board.getPile(board.getHelpFrom()-10).size()-1)*Pile.pPart);
                        
                        tx = 3*(Card.cardWidth+10)+10+(board.getHelpTo()-20)*(Card.cardWidth+10);
                        ty = Card.cardHeight+10;
                    }else {
                        // Pile card to pile
                        height = Card.cardHeight+ (board.getHelpK())*Pile.pPart;
                        fx = 10+(board.getHelpFrom()-10)*(Card.cardWidth+10);
                        fy = 2*(Card.cardHeight+10)+((board.getPile(board.getHelpFrom()-10).size()-1)*Pile.pPart);
                        
                        tx = 10+(board.getHelpTo()-10)*(Card.cardWidth+10);
                        ty = 2*(Card.cardHeight+10)+((board.getPile(board.getHelpTo()-10).size()+board.getHelpK())*Pile.pPart);
                    }
                }
                
                // Actual drawing
                temp = new java.awt.Color(255, 0, 0,180);
                g.setColor(temp);
                g.setFont(new Font("Tahoma", Font.BOLD, (int)(Card.cardWidth*0.25)));
                g.fillRect(fx, fy-height, Card.cardWidth, height);
                g.fillRect(tx, ty-height, Card.cardWidth, height);
                // Add text overlay
                g.setColor(new java.awt.Color(105, 10, 10));
                g.drawString("FROM", fx+(int)(Card.cardWidth*0.06), fy-(int)(Card.cardHeight*0.4));
                g.drawString("TO", tx+(int)(Card.cardWidth*0.3), ty-(int)(Card.cardHeight*0.4));
                
            }
            
            // Drag & Drop implementation
            if(follow) {
                if(from == 5 && !board.getPack().isEmpty()) {
                    if(board.getPack().isEmpty()) return;
                    board.getPack().get().draw(g, x-fx, y-fy);
                }else if(from >= 10 && from <= 19) {
                    if(board.getPile(from-10).isEmpty()) return;
                    board.getPile(from-10).get(board.getPile(from-10).get(cardNum)).draw(g, x-fx, y-fy, 0);
                }else if(from >= 20 && from <= 24) {
                    if(board.getTarget(from-20).isEmpty()) return;
                    board.getTarget(from-20).get().draw(g, x-fx,y-fy);
                }
            }
            
        }             
    }
   
    
    //Mouse adapter class

    /**
     * Handles clicking on the frame
     */
    public class MyMouseAdapter extends MouseAdapter{
    
        /**
         * Gets triggered when mouse is pressed and moving around the screen.
         * @param e event
         */
        @Override
            public void mouseDragged(MouseEvent e) {
                // For drag & drop purposes
                if (follow) {
                    x= e.getX();
                    y= e.getY();
                    repaint();
                } else {
                }
            }

        /**
         * Gets triggered on mouse press.
         * @param e event
         */
        @Override
            public void mousePressed(MouseEvent e) {
                help = false;
                to = 0;
                from = 0;
                cardNum = 0;
                x = e.getX();
                y = e.getY();
                //System.out.println("X: "+ x + ", Y: "+ y);

                int w = Card.cardWidth;
                int h = Card.cardHeight;

                if(game){
                    
                    // Help button
                    //g.fillRect(2*(Card.cardWidth + 10)+10, 10, Card.cardWidth, Card.cardHeight/2-5);
                    if(x >= 2*(Card.cardWidth + 10)+10 && x <= 3*(Card.cardWidth + 10) && y >= 10 && y <= 10+Card.cardHeight/2-5) {
                        //SHOW HELP
                        help = true;
                        board.help();
                        repaint();
                    }

                    // Undo button
                    if(x >= 2*(Card.cardWidth + 10)+10 && x <= 3*(Card.cardWidth + 10) && y >= 15+Card.cardHeight/2 && y <= 20+Card.cardHeight/2-5+Card.cardHeight/2-5) {
                        //UNDO MOVE
                        board.undo();
                        repaint();
                    }

                    // Click on pack next
                    if(x >= 10 && x <= w+10 && y >= 10 && y <= h+10) {
                        //System.out.println("Pack CLICK");
                        board.packNext();
                        highlight = true;
                        hsize = 1;
                        hx = 10;
                        hy = h+10;
                        repaint();
                    }

                    // Click on Pack
                    if(x >= w+20 && x <= 2*(w+10) && y >= 10 && y <= h+10) {
                        //System.out.println("tempPack CLICK");
                        from = 5;
                        cardNum = 0;
                        highlight = true;
                        hsize = 1;
                        hx = w+20;
                        hy = h+10;
                        follow = true;
                        fx= x-(w+20);
                        fy= y-(10);
                        repaint();
                    }

                    // Click on Piles
                    for (int i= 0; i < 7; i++) {
                        if(x >= 10+i*(w+10) && x <= (i+1)*(w+10) && y >= 20+h && y <= calcH(board.getPile(i).size())) {
                            //System.out.println("Pile "+ i +" CLICK");
                            from = 10+i;
                            cardNum = cardNrClicked(x,y,i);
                            highlight = true;
                            hsize = board.getPile(i).size();
                            hx = 10+i*(w+10);
                            hy = calcH(board.getPile(i).size());

                            follow = true;
                            fx= x-(10+i*(w+10));
                            fy=y - (20+h+(cardNum)*Pile.pPart);
                            repaint();
                        }
                    }

                    // Click on Targets
                    for (int i = 0; i < 4; i++) {
                        if(x >= (3+i)*(w+10)+10 && x <= (4+i)*(w+10) && y >= 10 && y <= h+10) {
                            //System.out.println("Target "+ i +" CLICK");
                            from = 20+i;
                            cardNum = 0;
                            highlight = true;
                            hsize = 1;
                            hx = (3+i)*(w+10)+10;
                            hy = h+10;

                            follow = true;
                            fx= x-((3+i)*(w+10)+10);
                            fy= y-(10);
                            repaint();
                        }
                    }
                }//closing game
                else {
                    // Click on Play Again
                    /*if(x >= getWidth()/2-150 && x <= getWidth()/2+150 && y >= getHeight()/2+50 && y <= getHeight()/2+100) {
                        System.out.println("CLICK Play Again");
                        game = true;
                        firstRun = false;
                        board = new Board();
                        repaint();
                    }*/
                }
            }

        /**
         * Gets triggered on mouse release.
         * @param e event
         */
        @Override
            public void mouseReleased(MouseEvent e) {
                int x = e.getX();
                int y = e.getY();
                //System.out.println("X: "+ x + ", Y: "+ y);
                highlight = false;
                follow = false;
                repaint();
                int w = Card.cardWidth;
                int h = Card.cardHeight;

                if(game) {
                    // Release on piles
                    for (int i= 0; i < 7; i++) {
                        if(x >= 10+i*(w+10) && x <= (i+1)*(w+10) && y >= 20+h && y <= calcH(board.getPile(i).size())) {
                            //System.out.println("Pile "+ i +" CLICK");
                            cardNrClicked(x,y,i);
                            to = 10 + i;
                        }
                    }

                    // Release on targets
                    for (int i = 0; i < 4; i++) {
                        if(x >= (3+i)*(w+10)+10 && x <= (4+i)*(w+10) && y >= 10 && y <= h+10) {
                            //System.out.println("Target "+ i +" CLICK");
                            to = 20 + i;
                        }
                    }

                    if(from != 0 && to != 0) {
                        // MoveCard calls -- board edits
                        if(from >=10 && from <=19 && to >=10 && to <= 19) {
                            //Pile to Pile
                            board.consoleLog();
                            board.moveCards(board.getPile(from-10), board.getPile(from-10).get(cardNum), board.getPile(to-10));
                            repaint();
                        }else if(from >=10 && from <=19 && to >=20 && to <= 24) {
                            //Pile to Target
                            //System.out.println("Pile "+ (from-10) +" to target "+ (to-20));
                            board.moveCards(board.getPile(from-10), board.getTarget(to-20));
                            repaint();
                        }else if(from == 5 && to >=10 && to <= 19) {
                            //Pack to Pile
                            board.moveCards(board.getPack(), board.getPile(to-10));
                            repaint();
                        }else if(from == 5 && to >=20 && to <= 29) {
                            //Pack to Target
                            //System.out.println("Pack to target "+ (to-20));
                            board.moveCards(board.getPack(), board.getTarget(to-20));
                            repaint();
                        }else if(from >=20 && from <=24 && to >=10 && to <= 19) {
                            //Target to Pile
                            board.moveCards(board.getTarget(from-20), board.getPile(to-10));
                            //System.out.println("Target to Pile");
                            repaint();
                        }

                        // Game Over check
                        if(!board.getTarget(0).isEmpty() && board.getTarget(0).get().value() == 13) {
                            if(!board.getTarget(1).isEmpty() && board.getTarget(1).get().value() == 13) {
                                if(!board.getTarget(2).isEmpty() && board.getTarget(2).get().value() == 13) {
                                    if(!board.getTarget(3).isEmpty() && board.getTarget(3).get().value() == 13) {
                                        System.out.println("GAME OVER.. YAAAY");
                                        game = false;
                                        repaint();
                                    }
                                }
                            }
                        }
                    }
                }// closing game
            }

            
        /**
         * 
         * @param s Size of pile
         * @return Height of pile in pixels
         */
            public int calcH(int s) {
                int ret = (s-1)*Pile.pPart + Card.cardHeight;
                //System.out.println("s"+ s+ "ret " + ret);
                return ret+20+Card.cardHeight;
            }

            // calculate which card of pile clicked

        /**
         *
         * @param x Position x of mouseClick
         * @param y Position y of mouseClicke
         * @param pile Pile clicked
         * @return number of card in pile that was clicked
         */
            public int cardNrClicked(int x, int y, int pile) {
                int w = Card.cardWidth;
                int h = Card.cardHeight;
                int size = board.getPile(pile).size();
                for (int i = 0; i < size; i++) {
                    if(i != size-1) {
                        if(y >= 20+h+(Pile.pPart*i) && y <= 20+h+(Pile.pPart*(i+1))) { 
                            //System.out.println("--- Card "+i+" clicked.");
                            return i;
                        }
                    }else {
                        if(y >= 20+h+(Pile.pPart*i) && y <= 20+h+(Pile.pPart*i)+h) {
                            //System.out.println("--- Top card clicked.");
                            return i;
                        }
                    }
                }
                return -10;
            }
        }
    
    /**
     * Saves current board in saves folder as serialized object
     * @param name Name of Game
     */
    public void saveBoard(String name) {
        if(!game) return;
        Timestamp timestamp = new Timestamp(System.currentTimeMillis());
        FileOutputStream fout = null;
        ObjectOutputStream oos = null;

        try {
            SimpleDateFormat format = new SimpleDateFormat("HH:mm:ss ] dd-MM-YYYY");

            String dateString = format.format(new Date());
            fout = new FileOutputStream("./examples/saves/"+name+" [ "+dateString+".sav");
            oos = new ObjectOutputStream(fout);
            oos.writeObject(this.board);

            System.out.println("Game Saved");

        } catch (Exception ex) {

            ex.printStackTrace();

        } finally {

            if (fout != null) {
                    try {
                            fout.close();
                    } catch (IOException e) {
                            e.printStackTrace();
                    }
            }

            if (oos != null) {
                    try {
                            oos.close();
                    } catch (IOException e) {
                            e.printStackTrace();
                    }
            }

        }
    }
    
    /**
     * Loads board object from save file into current game
     * @param filename filename of save file
     */
    public void loadBoard(String filename) {
        this.newGame();
        Board b = null;

        FileInputStream fin = null;
        ObjectInputStream ois = null;

        try {
                fin = new FileInputStream(filename);
                ois = new ObjectInputStream(fin);
                b = (Board) ois.readObject();
        } catch (Exception ex) {
                ex.printStackTrace();
        } finally {
            if (fin != null) {
                    try {
                            fin.close();
                    } catch (IOException e) {
                            e.printStackTrace();
                    }
            }

            if (ois != null) {
                    try {
                            ois.close();
                    } catch (IOException e) {
                            e.printStackTrace();
                    }
            }
        }
        this.board = b;
        System.out.println("Game Loaded.");
        repaint();
    }
}
