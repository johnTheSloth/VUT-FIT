package ija.ija2016;

import ija.ija2016.model.cards.CardDeck;
import java.io.Serializable;
import java.util.ArrayList;

/**
 * Handle states of board to reproduce undo moves
 * @author xgross09
 */
public class UndoManager implements Serializable{
    // stack-like structure of undo states
    ArrayList<State> history = new ArrayList();
    
   
    /**
     * Add undo state to history arraylist.
     * @param from From which cardDeck
     * @param to to which cardDeck
     */
    public void addUndo(CardDeck from, CardDeck to) {
        State temp = new State(from, to);
        history.add(temp);
    }
    
    /**
     * Remove last undo state from stack.
     */
    public void rmUndo() {
        history.remove(history.size()-1);
    }
    
    /**
     * Forcepush last state from history of undo's into game
     * and remove last undo.
     */
    public void executeUndo() {
        if(history.isEmpty()) return;
        CardDeck tempF = history.get(history.size()-1).getFromP();
        tempF.forcePutDeck(history.get(history.size()-1).getFrom());
        
        CardDeck tempT = history.get(history.size()-1).getToP();
        tempT.forcePutDeck(history.get(history.size()-1).getTo());
        rmUndo();
    }
    
}
