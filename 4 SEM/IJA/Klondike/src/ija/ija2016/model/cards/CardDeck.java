package ija.ija2016.model.cards;

import java.io.Serializable;
import java.util.ArrayList;

/**
 * Deck of cards representation class
 * @author xhlipa01
 */
public class CardDeck implements Serializable{

    /**
     * Deck of cards
     */
    protected ArrayList<Card> deck = new ArrayList();
    
    /**
     * Puts deck of cards without checking its value or color
     * @param deck Where to put
     */
    public void forcePutDeck(ArrayList<Card> deck) {
        this.deck = deck;
    }
    
    /**
     * Basic getter with fancy name
     * @return ArrayList
     */
    public ArrayList<Card> forceGetDeck() {
        return deck;
    }
    
    /**
     * 
     * @return size of deck
     */
    public int size() {
        return deck.size();
    }
    
    /**
     * Puts card to CardDeck
     * @param card Card
     * @return True on success
     */
    public boolean put(Card card) {
        deck.add(deck.size(), card);
        return true;
    }
    
    /**
     * Put's card to CardDeck withou checking, and with fancy name
     * @param card Card
     * @return True on success
     */
    public boolean forcePutCard(Card card) {
        deck.add(deck.size(), card);
        return true;
    }
    
    /**
     * Removes the card.
     * @return Top card of stack
     */
    public Card pop() {
        if(deck.isEmpty()) return null;
        return deck.remove(deck.size()-1);
    }
    
    /**
     * Doesn't remove the card.
     * @return Top card of stack
     */ 
    public Card get() {
      if(deck.isEmpty()) return null;
      return deck.get(deck.size()-1);
    }
    
    /**
     *
     * @param index Position of card
     * @return Card on provided index
     */
    public Card get(int index) {
        if(deck.isEmpty()) return null;
        return deck.get(index);
    }
    
    /**
     * Says if cardDeck is empty(true) or not(false).
     * @return True/False
     */
    public boolean isEmpty() {
        return deck.isEmpty();
    }
}
