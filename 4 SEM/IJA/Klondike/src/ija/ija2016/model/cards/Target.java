package ija.ija2016.model.cards;

import java.awt.Graphics;
import java.io.Serializable;

/**
 * Target representation class
 * @author xhlipa01
 */
public class Target extends CardDeck implements Serializable{
    private Card.Color color;

    /**
     * Color setter
     * @param color Color
     */
    public void setColor(Card.Color color) {
        this.color = color;
    }
        
    /**
     * Force put card into target, no checks
     * @param card Card
     * @return True
     */
    public boolean forcePut(Card card) {
        deck.add(deck.size(), card);
        return true;
    }
        
    /**
     * Adds card to Target, checks if it's not against rules of game.
     * @param card Card
     * @return True on success
     */
    @Override
    public boolean put(Card card) {
        if(this.isEmpty() && 1 == card.value()) {
            setColor(card.color());
            return deck.add(card);
        }else if(card.color() == color && this.size() == card.value()-1){
            return deck.add(card);
        }
        return false;
    }
    
    /**
     * Main drawing function
     * @param g Graphics
     * @param frameOffX X position of target section
     * @param frameOffY Y position of target section
     * @param off offset of current target
     */
    public void draw(Graphics g, int frameOffX, int frameOffY, int off) {
        int offLocal = 0;
        if(this.deck.isEmpty()) {
            //placeholder
            java.awt.Color temp = new java.awt.Color(255,255,255,30);
            g.setColor(temp);
            g.fillRect(frameOffX+off, frameOffY, Card.cardWidth, Card.cardHeight);
            g.setColor(java.awt.Color.BLACK);
            //g.drawString("PLACEHOLDER", frameOffX+off+5, frameOffY+25);
        }else {
            deck.get(deck.size()-1).draw(g, frameOffX+off, frameOffY+offLocal);
        }
    }
}
