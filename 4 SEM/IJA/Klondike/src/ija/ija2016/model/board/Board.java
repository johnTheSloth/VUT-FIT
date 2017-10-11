/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package ija.ija2016.model.board;

import ija.ija2016.UndoManager;
import ija.ija2016.model.cards.Card;
import ija.ija2016.model.cards.Pack;
import ija.ija2016.model.cards.Pile;
import ija.ija2016.model.cards.Target;
import java.io.Serializable;

/**
 * Board representation class
 * @author xgross09
 */
public class Board implements Serializable{
    private static final long serialVersionUID = 1L;
    
    /**
     *  Pack of cards in top left corner, not clickable
     */
    protected Pack pack;

    /**
     * Pack of cards taken from Pack
     */
    protected Pack tempPack;

    /**
     * Array of 7 piles
     */
    protected Pile[] piles;

    /**
     * Array of 4 targets
     */
    protected Target[] targets;   

    /**
     * Undo handler
     */
    protected UndoManager hist;

    /**
     * keycode of FROM position of card
     */
    protected int helpFrom;

    /**
     * keycode of TO position of card
     */
    protected int helpTo;

    /**
     * keycode of card position in PILE
     */
    protected int helpK;

    //Getters

    /**
     *
     * @return int helpK
     */
    public int getHelpK() {
        return helpK;
    }

    /**
     *
     * @return int helpFrom
     */
    public int getHelpFrom() {
        return helpFrom;
    }

    /**
     *
     * @return int helpTO
     */
    public int getHelpTo() {
        return helpTo;
    }
    
    /**
     * Returns 1 pile from piles array
     * @param num Number
     * @return Pile x
     */
    public Pile getPile(int num) {
        if(num > 6) return null;
        return piles[num];
    }
    
    /**
     * Returns 1 target from targets aray
     * @param num Number
     * @return Target x
     */
    public Target getTarget(int num) {
        if(num > 4) return null;
        return targets[num];
    }
    
    /**
     * Returns tempPack
     * @return Pack tempPack
     */
    public Pack getPack() {
        return tempPack;
    }

    /**
     * Basic board constructor
     */
    public Board() {
        hist = new UndoManager();
        pack = new Pack();
        tempPack = new Pack();
        // init 52 cards cardpack
        pack = pack.createPack();
        pack.shuffle();
         
        // create and init 7 instances of Pile
        piles = new Pile[7];
        for (int i = 0; i < 7; i++) {
            piles[i] = new Pile();
        }
        fillPiles();
        
        // create and init 4 target packs, 1 for each color
        targets = new Target[4];
        for (int i = 0; i < 4; i++) {
            targets[i] = new Target();
        }
        
        // turn face up of top card in Pack
        pack.get().turnFaceUp();
    }
    
    /**
     * Triggers undo
     */
    public void undo() {
        hist.executeUndo();
    }
    
    // Fill piles with cards from pack
    private void fillPiles() {
        for (int i = 1; i <= 7; i++) {
            for (int j = 0; j < i; j++) {
                piles[i-1].initPut(pack.pop());
            }
            piles[i-1].get().turnFaceUp();
        }
    }
    
    // MOVES:
    // Pile to Pile

    /**
     * Moves card from pile to pile
     * @param from Pile
     * @param card Card
     * @param to Pile
     * @return True on success
     */
    public boolean moveCards(Pile from, Card card, Pile to) {
        if(from.isEmpty()) return false;
        Pile temp = from.get(card);
        hist.addUndo(from, to);
        if(to.put(temp)) {
            from.pop(card);
            if(!from.isEmpty()) {
                from.get().turnFaceUp();
            }
            return true;
        }else {
            System.out.println("Invalid move. Pile to Pile");
            hist.rmUndo();
        }
        return false;
    }
    
    //Pile to Target

    /**
     * Moves card from Pile to Target
     * @param from Pile
     * @param to Target
     * @return True on success
     */
    public boolean moveCards(Pile from, Target to) {
        if(from.isEmpty()) return false;
        hist.addUndo(from, to);
        if(to.put(from.get())) {
            from.pop();
            if(!from.isEmpty()) {
                from.get().turnFaceUp();
            }
            return true;
        }else {
            hist.rmUndo();
            System.out.println("Invalid move. Pile to Target");
        }
        return false;
    }
    
    //Pack to Pile

    /**
     * Moves card from Pack to Pile
     * @param from Pack
     * @param to Pile
     * @return True on success
     */
    public boolean moveCards(Pack from, Pile to) {
        if(from.isEmpty()) return false;
        hist.addUndo(from, to);
        if(to.put(from.get())) {
            from.pop();
            if(!from.isEmpty()) {
                from.get().turnFaceUp();
            }
            return true;
        }else {
            hist.rmUndo();
            System.out.println("Invalid move. Pack to Pile");
        }
        return false;
    }
    
    //Pack to Target

    /**
     * Moves card from Pack to Target
     * @param from Pack
     * @param to Target
     * @return True on success
     */
    public boolean moveCards(Pack from, Target to) {
        if(from.isEmpty()) return false;
        hist.addUndo(from, to);
        if(to.put(from.get())) {
            from.pop();
            if(!from.isEmpty()) {
                from.get().turnFaceUp();
            }
            return true;
        }else {
            hist.rmUndo();
            System.out.println("Invalid move. Pack to Target");
        }
        return false;
    }
    
    //Target to Pile

    /**
     * Moves card from Target to Pile
     * @param from Target
     * @param to Pile
     * @return True on success
     */
    public boolean moveCards(Target from, Pile to) {
        if(from.isEmpty()) return false;
        hist.addUndo(from, to);
        if(to.put(from.get())) {
            from.pop();
            return true;
        }else {
            hist.rmUndo();
            System.out.println("Invalid move. Target to Pile");
        }
        return false;
    }
    
    /**
     * Get another card from pack
     */
    public void packNext() {
        if(pack.isEmpty()) {
            int size = tempPack.size();
            for (int i = 0; i < size; i++) {
                if(pack.put(tempPack.pop())) {
                }else {
                }
            }
        }else {
            hist.addUndo(tempPack, pack);
            tempPack.put(pack.pop());
            tempPack.get().turnFaceUp();
        }
    }
    

    /**
     * Compute next best move 
     * ---- list of good moves:
     * Pack to target
     * Pack to pile
     * Last turned up Pile to pile
     * Top pile to target
     * Next card from pack
     */

    public void help() {
        helpFrom = 0;
        helpK = 0;
        helpTo = 0;
        
        for (int i = 0; i < 4; i++) {
            //pack [A] to target
            if(!this.getPack().isEmpty() && this.getPack().get().value() == 1 && this.getTarget(i).isEmpty()) {
                helpFrom = 5;
                helpTo = 20+i;
                return;
            }
            if(1==1) {
                //pack card to target
                if(!this.getTarget(i).isEmpty() && !this.getPack().isEmpty() && this.getPack().get().value() == this.getTarget(i).get().value()+1 && this.getPack().get().color() == this.getTarget(i).get().color()) {
                    helpFrom = 5;
                    helpTo = 20+i;
                    return;
                }
                for (int j = 0; j < 7; j++) {
                    if(!this.getPack().isEmpty() && this.getPack().get().value() == 13 && this.getPile(j).isEmpty()) {
                        helpFrom = 5;
                        helpTo = 10+j;
                        return;
                    }
                    if(!this.getPile(j).isEmpty()) {
                        //pack [K] to empty pile
                        //pile card to target
                        if(!this.getTarget(i).isEmpty() && this.getPile(j).get().value() == this.getTarget(i).get().value()+1 && this.getPile(j).get().color() == this.getTarget(i).get().color()) {
                            helpFrom = 10+j;
                            helpTo = 20+i;
                            return;
                        }
                        //[A] from pile to target
                        if(this.getPile(j).get().value() == 1 && this.getTarget(i).isEmpty()) {
                            helpFrom = 10+j;
                            helpTo = 20+i;
                            return;
                        }
                        int k = 0;
                        while(!this.getPile(j).get(k).isTurnedFaceUp()) {
                            k++;
                        }
                        //pile to pile
                        for (int l = 0; l < 7; l++) {
                            if(!this.getPile(l).isEmpty() && this.getPile(j).get(k).value() == this.getPile(l).get().value()-1 && !this.getPile(j).get(k).similarColorTo(this.getPile(l).get())) {
                                helpFrom = 10+j;
                                helpTo = 10+l;
                                helpK = this.getPile(j).size()-1-k;
                                return;
                            }
                            if(k >=1 && !this.getPile(j).get(k-1).isTurnedFaceUp()) {
                                
                                if(this.getPile(j).get(k).value() == 13 && this.getPile(l).isEmpty()) {
                                    helpFrom = 10+j;
                                    helpTo = 10+l;
                                    helpK = this.getPile(j).size()-1-k;
                                    return;
                                }
                            }
                            
                        }
                        //pack card to pile
                        if(!this.getPack().isEmpty() && this.getPack().get().value() == this.getPile(j).get().value()-1 && !this.getPack().get().similarColorTo(this.getPile(j).get())) {
                            helpFrom = 5;
                            helpTo = 10+j;
                            return;
                        }
                        
                    }
                }
            }
            
        }
    } // end of help()
    
    // Console version info - prints all piles
    private void printPiles() {
        System.out.println("PILES:");
        for (int i = 0; i < 7; i++) {
            for (int j = 0; j < piles[i].size(); j++) {
                System.out.printf(""+piles[i].get(j).toString()+", ");
            }
            System.out.println("");
        }
    }
    
    // Console version info - prints all targets
    private void printTargets() {
        System.out.println("TARGETS:");
        for (int i = 0; i < 4; i++) {
            if(targets[i].isEmpty()) {
                System.out.println("[ ]");
            }else {
                System.out.println("["+targets[i].get().toString()+"]");
            }
        }
    }
    
    // Console version info - prints pack
    private void printPack() {
        System.out.println("PACK:");
        if(!tempPack.isEmpty()) {
            System.out.println(""+tempPack.get().toString());
        }
    }

    /**
     * Console version info - triggers all prints (pack, piles, targets)
     */
    public void consoleLog() {
        printTargets();
        printPack();
        printPiles();
    }
}
