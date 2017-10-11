/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package ija.ija2016;

import ija.ija2016.model.cards.Card;
import ija.ija2016.model.cards.CardDeck;
import java.io.Serializable;
import java.util.ArrayList;

/**
 * State class is used to save current board state, is used by UndoManager.
 * @author xgross09
 */
public class State implements Serializable{

    /**
     * FROM position
     */
    protected CardDeck fromP;

    /**
     * FROM position
     */
    protected ArrayList<Card> from;

    /**
     * TO position
     */
    protected CardDeck toP;

    /**
     * TO position
     */
    protected ArrayList<Card> to;
    
    /**
     *
     * @param from From
     * @param to To
     */
    public State(CardDeck from, CardDeck to) {
        this.fromP = from;
        this.from = new ArrayList<>();
        for(Card c : from.forceGetDeck()) {
            this.from.add(c.clone());
        }
        this.toP = to;
        this.to = new ArrayList<>();
        for(Card c : to.forceGetDeck()) {
            this.to.add(c.clone());
        }
    }
    
    /**
     *
     * @return ArrayList from where is part picked.
     */
    public ArrayList<Card> getFrom() {
        return from;
    }
    
    /**
     *
     * @return Array list to which is part pushed.
     */
    public ArrayList<Card> getTo() {
        return to;
    }
    
    /**
     *
     * @return CardDeck from where is part picked.
     */
    public CardDeck getFromP() {
        return fromP;
    }
    
    /**
     *
     * @return CardDeck to which is part pushed.
     */
    public CardDeck getToP() {
        return toP;
    }
}
