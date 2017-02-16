/**
 * Interface cho viec lang nghe su kien.
 */
package org.cocos2dx.cpp.googlegameservices;

/**
 * @author Tam Dinh
 */
public interface IActionListener {
	
    /**
     * 
     * Thá»±c hiá»‡n má»™t Action
     * @param o : Gá»“m 2 pháº§n
     * - Object : Source command ( Command nÃ o cho cÃ¡i nÃ y -> Ä‘á»ƒ gá»™p láº¡i xá»­ lÃ½ chung).
     * - Object : Source component ( Widget nÃ o thá»±c hiá»‡n hÃ nh Ä‘á»™ng nÃ y -> data).
     * NghÄ©a lÃ  : o = new Object{Src command, src widget }
     * CÃ³ thá»ƒ Ä‘á»‹nh nghÄ©a cho cÃ¡c trÆ°á»�ng há»£p Ä‘áº·c biá»‡t khÃ¡c.
     * 
     */
    public void actionPerformed(Object o);
           
}
