package ija.ija2016.model.cards;

import java.awt.Graphics;
import java.io.Serializable;
import java.util.Collections;

/**
 * Pack representation class
 * @author xhlipa01
 */
public class Pack extends CardDeck implements Serializable{
    
    /**
     * Create pack of 52 cards, 13 for each color.
     * @return Created pack.
     */
    public Pack createPack() {
        Pack standard = new Pack();
        for (Card.Color color : Card.Color.values()) {
            for (int j = 1; j < 14; j++) {
                Card c = new Card(color, j);
                standard.put(c);
            }
        }
        return standard;
    }
    
    /**
     * Shuffles the pack.
     */
    public void shuffle() {
        Collections.shuffle(deck);
    }
    
    /**
     * Main drawing function
     * @param g Graphics
     * @param frameOffX X position of card on screen
     * @param frameOffY Y position of card on screen
     */
    public void draw(Graphics g, int frameOffX, int frameOffY) {
        int offLocal = 0;
        if(this.deck.isEmpty()) {
            //placeholder
            //java.awt.Color temp = new java.awt.Color(255,255,255);
            //g.setColor(temp);
            //g.fillRect(frameOffX, frameOffY, Card.cardWidth, Card.cardHeight);
            //g.setColor(java.awt.Color.BLACK);
            //g.drawString("PACK", frameOffX+5, frameOffY+25);
        }else {
            get().draw(g, frameOffX, frameOffY);
        }
       
        
    }
}
