/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package ija.ija2016.model.cards;

import java.awt.Font;
import java.awt.Graphics;
import java.io.Serializable;
import java.util.Objects;

/**
 * Card representation class
 * @author xhlipa01
 */
public class Card implements Serializable{

    /**
     * Card color representation,
     * CLUBS C,
     * HEARTS H,
     * DIAMONDS D,
     * SPADES S,
     */
    public static enum Color {

        /**
         * Krize
         */
        CLUBS,

        /**
         * Kary
         */
        DIAMONDS,

        /**
         * Srdce
         */
        HEARTS,

        /**
         * Piky
         */
        SPADES;
        
        /**
         *
         * @return First letter of Color (C H D S)
         */
        @Override
        public String toString() {
          switch(this) {
            case CLUBS: return "C";
            case HEARTS: return "H";
            case DIAMONDS: return "D";
            case SPADES: return "S";
            default: throw new IllegalArgumentException();
          }
        }
    }
    
    private Color color;
    private int value;
    private boolean faceUp;

    /**
     * Height of card
     */
    public static int cardHeight = 190;

    /**
     * Width of card
     */
    public static int cardWidth = 100;

    /**
     * Sets height of card
     * @param h Height in pixels
     */
    public static void setHeight(int h) {
        Card.cardHeight = h;
    }
    
    /**
     * Sets width of card
     * @param w Width in pixels
     */
    public static void setWidth(int w) {
        Card.cardWidth = w;
    }
    
    // Main usage in console version

    /**
     * Get card code in format :Number Color
     * @return value of card and color
     */
    @Override
    public String toString() {
        String ret;
        if(isTurnedFaceUp()) {
            ret = "{ } ";
        }else {
            ret = "{X} ";
        }
        
        switch (this.value) {
            case 11:
                ret += " J";
                break;
            case 12:
                ret += " Q";
                break;
            case 13:
                ret += " K";
                break;
            case 1:
                ret += " A";
                break;
            case 10:
                ret += value;
                break;
            default:
                ret += " "+value;
                break;
        }
        ret += " "+color;
        
        return ret;
    }
    
    /**
     *
     * @return String made of card value
     */
    public String strVal() {
        String ret;
        switch (this.value) {
            case 11:
                ret = " J";
                break;
            case 12:
                ret = " Q";
                break;
            case 13:
                ret = " K";
                break;
            case 1:
                ret = " A";
                break;
            case 10:
                ret = ""+value;
                break;
            default:
                ret = " "+value;
                break;
        }
        return ret;
    }
    
    /**
     *  Getter of card height
     * @return card height
     */
    public int getHeight() {
        return this.cardHeight;
    }
    

    /**
     * Drawing of card object
     * @param g Graphics
     * @param x Coordinate x
     * @param y Coordinate y
     */
    public void draw(Graphics g, int x, int y) {
        java.awt.Color red = java.awt.Color.decode("0xce0000");
        java.awt.Color black = java.awt.Color.decode("0x1c1c1c");
        java.awt.Color white = java.awt.Color.decode("0xededed");
        
        
        java.awt.Color temp;
        switch(this.color) {
            case CLUBS: temp = black;break;
            case HEARTS: temp = red;break;
            case DIAMONDS: temp = red;break;
            case SPADES: temp = black;break;
            default: throw new IllegalArgumentException();
        }
        // if card is not turned face up, draw back of the card
        if(!isTurnedFaceUp()){
            g.setColor(java.awt.Color.DARK_GRAY);
            g.fillRect(x, y, Card.cardWidth,Card.cardHeight);
            g.setColor(new java.awt.Color(239, 207, 151));
            g.fillRect(x+1, y+1, Card.cardWidth-1, Card.cardHeight-2);
            g.setColor(new java.awt.Color(140, 0, 12,40));
            g.fillRect(x+2, y+2, Card.cardWidth-2, Card.cardHeight-3);
            g.setColor(new java.awt.Color(237, 150, 0,30));
            g.fillRect(x+Card.cardWidth-40, y, 30, Card.cardHeight-5);
        }else {
            // draw face of card
            g.setColor(java.awt.Color.DARK_GRAY);
            g.fillRect(x, y, Card.cardWidth,Card.cardHeight);
            g.setColor(temp);
            g.fillRect(x+1, y+1, this.cardWidth-1, this.cardHeight-1);
            g.setColor(new java.awt.Color(140, 0, 12,40));
            g.fillRect(x+2, y+2, Card.cardWidth-2, Card.cardHeight-3);
            g.setColor(white);
            g.fillRect(x+3, y+3, this.cardWidth-6, this.cardHeight-6);
            g.setColor(temp);
            g.setFont(new Font("TimesRoman", Font.BOLD, (int)(this.cardWidth*0.25)));
            String sign;
            switch(this.color) {
                case CLUBS: sign = "♣";break;
                case HEARTS: sign = "♥";break;
                case DIAMONDS: sign = "♦";break;
                case SPADES: sign = "♠";break;
                default: throw new IllegalArgumentException();
            }
            // Write value TOP LEFT
            g.drawString(this.strVal(), x, y+(int)(Card.cardWidth*0.25));
            // Write signt TOP right
            g.drawString(sign, x+(int)(Card.cardWidth*0.7), y+(int)(Card.cardWidth*0.25));
            // write value BOT Right
            g.drawString(this.strVal(), x+(int)(Card.cardWidth*0.65), y+(int)(Card.cardHeight*0.95));
            // write sign bot left
            g.drawString(sign, x+(int)(Card.cardWidth*0.1), y+(int)(Card.cardHeight*0.95));
            g.setColor(temp);
            g.setFont(new Font("TimesRoman", Font.BOLD, (int)(Card.cardWidth*0.5)));
            g.drawString(sign, x+(int)(Card.cardWidth*0.32), y+(int)(Card.cardHeight*0.65));
        }
    }
    
    /**
     *
     * @return new instance of card
     */
    public Card clone() {
        Card c = new Card(this.color, this.value);
        c.faceUp = this.faceUp;
        return c;
    }
    
    /**
     * Basic constructor
     * @param c color of card
     * @param value value of card
     */
    public Card(Card.Color c, int value) {
        this.color = c;
        this.value = value;
    }
    
    /**
     * Getter of value
     * @return value of card
     */
    public int value() {
        return value;
    }

    /**
     * Returns true if card is turned face up.
     * @return True if card is faceing up
     */
    public boolean isTurnedFaceUp() {
        return faceUp;
    }

    /**
     *
     * @return True if card was turned face up else fals if card was already turned face up.
     */
    public boolean turnFaceUp() {
        if (!faceUp) {
            faceUp = true;
            return true;
        }
        return false;
    }

    /**
     * Card color getter
     * @return Color of card.
     */
    public Color color() {
        return color;
    }

    /**
     * Checks if card has similar color (Red/Black) to another card
     * @param c Card to check
     * @return true or false
     */
    public boolean similarColorTo(Card c) {
        if((c.color() == color.HEARTS || c.color() == color.DIAMONDS) && (this.color() == color.HEARTS || this.color() == color.DIAMONDS)){
           return true;
        }else if((c.color() == color.SPADES || c.color() == color.CLUBS) && (this.color() == color.SPADES || this.color() == color.CLUBS)){
           return true;
        }
        return false;
    }

    /**
     * Checks if card c has bigger or lesser value
     * @param c Card
     * @return this.value - c.value
     */
    public int compareValue(Card c) {
        return this.value - c.value();
    }

    /**
     *
     * @return hashed card
     */
    @Override
    public int hashCode() {
        int hash = 3;
        hash = 59 * hash + Objects.hashCode(this.color);
        hash = 59 * hash + this.value;
        hash = 59 * hash + (this.faceUp ? 1 : 0);
        return hash;
    }

    /**
     *
     * @param obj Object to compare
     * @return true or false
     */
    @Override
    public boolean equals(Object obj) {
        if (this == obj) {
            return true;
        }
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final Card other = (Card) obj;
        if (this.value != other.value) {
            return false;
        }
        if (this.faceUp != other.faceUp) {
            return false;
        }
        if (this.color != other.color) {
            return false;
        }
        return true;
    }
    


}
