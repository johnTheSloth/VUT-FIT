package ija.ija2016.model.cards;

import java.awt.Graphics;
import java.io.Serializable;
import java.util.ArrayList;

/**
 * Pile representation class
 * @author xhlipa01
 */
public class Pile extends CardDeck implements Serializable{

    /**
     * pPart value setter
     * @param pPart Part
     */
    public static void setpPart(int pPart) {
        Pile.pPart = pPart;
    }

    /**
     * Height of visible part of card while in Pile
     */
    static public int pPart = (int)Card.cardHeight/7;

    /**
     * Basic constructor
     */
    public Pile() {
        //this.deck = new ArrayList();
    }
    
    /**
     * Puts "array" of cards onto Pile
     * @param part "Array" of cards
     * @return True on success
     */
    public boolean put(Pile part) {
       if(isEmpty() && part.get(0).value() == 13) {
           this.deck.addAll(part.deck);
           return true;
       }
       if(isEmpty() || part.isEmpty()) return false;
       if(this.get().value() == part.get(0).value()+1 && part.get(0).isTurnedFaceUp() && !this.get().similarColorTo(part.get(0))) {
           this.deck.addAll(part.deck);
           return true;
       }
        System.out.println("This val "+ this.get().value());
        System.out.println("part " + part.get(0).value());
       return false;
    }
    
    /**
     * Add desired card to pile.
     * @param c Card to be added on Pile
     * @return True on success
     */
    @Override
    public boolean put(Card c) {
       if(isEmpty() && c.value() == 13) {
           this.deck.add(c);
           return true;
       }
       if(this.get().value() == c.value()+1 && c.isTurnedFaceUp() && !this.get().similarColorTo(c)) {
           this.deck.add(c);
           return true;
       }
       return false;
    }

    /**
     * Return "array" (pile) of cards from top of Pile to desired card.
     * Removes it from Pile.
     * @param card Desired card.
     * @return Pile part
     */
    public Pile pop(Card card) {
        Pile ret = new Pile();
        int pos = deck.indexOf(card);
        if(pos == -1) return null;
        
        while(pos < deck.size()) {
            ret.deck.add(ret.deck.size(), this.deck.get(pos));
            this.deck.remove(pos);
        }

        return ret;
    }
    
    /**
     * Return "array" (pile) of cards from top of Pile to desired card.
     * @param card Desired card.
     * @return Pile part
     */
    public Pile get(Card card) {
        ArrayList<Card> dupe = new ArrayList<>(deck);
        Pile ret = new Pile();
        int pos = dupe.indexOf(card);
        if(pos == -1) return null;
        
        //int i = deck.size();
        while(pos < dupe.size()) {
            ret.forcePutCard(dupe.get(pos));
            dupe.remove(pos);
        }

        return ret;
    }
    
    /**
     * Main drawing function of Pile.
     * @param g Graphics
     * @param frameOffX X position of pile sector
     * @param frameOffY Y position of pile sector
     * @param off Y offset of current pile
     */
    public void draw(Graphics g, int frameOffX, int frameOffY, int off) {
        // update part height
        setpPart((int)Card.cardHeight/7);
                
        int offLocal = 0;
        if(this.deck.isEmpty()) {
            //placeholder
            java.awt.Color temp = new java.awt.Color(255,255,255,30);
            g.setColor(temp);
            g.fillRect(frameOffX+off, frameOffY, Card.cardWidth, Card.cardHeight);
            g.setColor(java.awt.Color.BLACK);
            //g.drawString("PLACEHOLDER", frameOffX+off+5, frameOffY+25);
        }else {
            for (Card c : this.deck) {
                c.draw(g, frameOffX+off, frameOffY+offLocal);
                offLocal += this.pPart;
            }
        }
    }

    /**
     * Force put of shuffled cards.
     * @param card Card
     * @return true
     */
    public boolean initPut(Card card) {
        deck.add(deck.size(), card);
        return true;
    }
}
