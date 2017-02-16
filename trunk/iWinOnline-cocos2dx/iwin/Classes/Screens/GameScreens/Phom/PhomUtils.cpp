#include "PhomUtils.h"
#include "PhomScreen.h"
#include "Screens/Object/CardUtils.h"


/**
 * check có bằng điểm ko
 */
bool PhomUtils::isSameValue(ubyte id0, ubyte id1) {
	return id0 / 4 == id1 / 4;
}

/**
 * check có phải điểm liên tiếp và cùng chất ko
 */
bool PhomUtils::isStraightAndSameType(ubyte id0, ubyte id1, int d) {
	return id0 / 4 + d == id1 / 4 && id0 % 4 == id1 % 4;
}

/**
 * check id có nằm trong array hay ko
 */
bool PhomUtils::isContains(VecCard cards, ubyte id)
{
	return false;
    //return cards.size() > 0 && K_VEC_CONTAIN(cards, id);
}


/**
* kiểm tra xem ăn được hay ko
*
* @param wantEatID
*            : card muốn ăn
* @param currentEatCards
*            != null
* @param currentDealCards
*            : các lá bài tự có và rút hiện tại
* @param needCards
*            : trả về các lá bài tạo thành phỏm với lá muốn ăn (2 lá)
* @param newPhoms
*            : trả về các phỏm mới được tạo theo lá muốn ăn
* @param newFreeCards
*            : trả về các bài rác mới
* @return
*/
bool PhomUtils::canEat(ubyte wantEatID, VecByte currentEatCards, VecByte currentDealCards, VecByte& needCards, std::vector<Phom>& newPhoms, VecByte& newFreeCards) {
	/* thuật toán giả sử ,ăn con bài wantEat vô (add vô eatCards), và tìm
	 hết phỏm, nếu ko còn
	 con bài ăn nào nằm ở ngoài thì ăn được*/
	newFreeCards.clear();
	needCards.clear();
	newPhoms.clear();

	VecByte tmpEatCards;
	K_VEC_INSERT(tmpEatCards, currentEatCards);
	tmpEatCards.push_back(wantEatID);

	CardUtils::sortByValue_CRChB(true, tmpEatCards);
	
	std::vector<Phom> find_phoms = findPhoms(currentDealCards, tmpEatCards, newFreeCards, true);
	K_VEC_INSERT(newPhoms, find_phoms);
    
	bool canEat = newPhoms.size() > 0;

	if (!canEat) {
		//Log.error(TAG_EAT_PHOM, "không thể ăn ! Không tạo được phỏm nào !");
		return false;
	}

	for (size_t j = 0; j < tmpEatCards.size(); j++) {
		if(K_VEC_CONTAIN(newFreeCards, tmpEatCards.at(j))){
			//Log.error(TAG_EAT_PHOM, "không thể ăn,có bài ăn nằm ngoài : "
			//	+ Card.card(tmpEatCards.get(j), TYPECARD));
			return false;
		}
	}
	 //neu co 2 bai an cung nam trong mot phom
	 //Log.log(TAG_EAT_PHOM, "eat phom: "+newPhoms);
	 //Log.log(TAG_EAT_PHOM, "wantEatID: "+wantEatID);
	 //Log.log(TAG_EAT_PHOM,
	 //"currentEatCards: "+currentEatCards.toString());
	if (!currentEatCards.empty()) {
		for (size_t i = 0; i < newPhoms.size(); i++) {
			for (size_t j = 0; j < currentEatCards.size(); j++)
				if(K_VEC_CONTAIN(newPhoms.at(i)._cards, wantEatID) &&
					K_VEC_CONTAIN(newPhoms.at(i)._cards, currentEatCards.at(j))) {
					//Log.log(TAG_EAT_PHOM,
					//	"không thể ăn do phỏm chứa đến 2 con bai an");
					return false;
				}
		}
	}

	// có thể ăn, tìm 2 con cần thiết
	Phom phom;
	for (size_t i = 0; i < newPhoms.size(); i++) {
		phom = newPhoms.at(i);
		if(K_VEC_CONTAIN(phom._cards, wantEatID)) {
			for (size_t j = 0; j < phom._cards.size(); j++) {
				if (phom._cards.at(j) != wantEatID) {
					needCards.push_back(phom._cards.at(j));
				}
			}
			break;
		}
	}
	//Log.log(TAG_EAT_PHOM, "ăn được");
	return canEat;
}

/**
 * coi xem các lá này có phải 1 phỏm ko (chỉ dùng để check 1 phỏm rời rạc)
 *
 * @param cards
 * @return
 */
int PhomUtils::getPhomTypeAlone(VecByte& cards,
                                VecByte eatCards) {
    CardUtils::sortByValue_CRChB(true, cards);
    // Log.log("check", Card.cards(cards, TYPECARD));
    bool ok = true;
    int num = 0;
    // check coi có chứa >1 lá bài ăn ko
    for (size_t i = 0; i < cards.size(); i++) {
        if (cards.at(i) > -1) {
            if(K_VEC_CONTAIN(eatCards, cards.at(i)))
                num++;
            if (num > 1) {
                ok = false;
                break;
            }
        }
    }
    if (!ok)
        return NONE;
    // check xem có phải phỏm 3 lá ko
    for (size_t i = 0; i < cards.size() - 1; i++) {
        if (!isSameValue(cards.at(i), cards.at(i + 1))) {
            ok = false;
            break;
        }
    }
    if (ok)
        return PHOM_3;
    // ko phải phỏm 3 lá,check xem có phải phỏm sảnh ko
    ok = true;
    
    for (size_t i = 1; i < cards.size(); i++) {
        if (!isStraightAndSameType(cards.at(0), cards.at(i), i)) {
            ok = false;
            break;
        }
    }
    if (ok)
        return PHOM_SANH;
    
    return NONE;
}

/**
 *
 * @param cards
 *            : 3 lá bài muốn tạo phỏm
 *
 * @param currentEatCards
 *            : bài ăn hiện tại
 * @param currentDealCards
 *            : bài trên tay hiện tại (bài lúc chia,bài rút)
 * @param newPhoms
 *            : trả về phỏm sau khi tạo (nếu thành công)
 * @param newFreeCards
 *            : bài rác mới
 * @return thành công hay ko
 */
bool PhomUtils::canMakePhoms(VecByte cards,
                             VecByte currentEatCards,
                             VecByte currentDealCards, std::vector<Phom>& newPhoms,
                             VecByte& newFreeCards, bool isAddMorePhom) {
    /*
     * thuật toán giả sử ,nếu 3 lá bài cards là 1 phỏm và remove 3 lá này ra
     * khỏi bộ bài(currentDealCards,eatCards). Sau đó tìm hết phỏm trong bộ
     * bài còn lại mà ko có lá bài dư nào là bài ăn thì có thể tạo phỏm
     */
    
    /*
     * trước tiên check 3 lá bài có phải là 1 phỏm hay ko
     */
    if (cards.size() < 3) {
        //Log.error(TAG_MAKE_PHOM, cards.size + " lá ko tạo được");
        return false;
    }
    int phomType = getPhomTypeAlone(cards, currentEatCards);
    if (phomType == NONE) {
        //Log.error(TAG_MAKE_PHOM, Card.cards(cards, TYPECARD)
        // + " không phải là phỏm !");
        return false;
    }
    
    // 3 lá này là 1 phỏm ,tiếp tục check xem nếu nó tạo thành phỏm thì bộ
    // bài hiện tại trừ nó ra tạo thành phỏm có dư lá bài ăn nào ko ?
    CardUtils::sortByValue_CRChB(true, cards);
    
    newFreeCards.clear();
    newPhoms.clear();
    
    // danh sách này chỉ chứa các lá bài tự có và ko chứa 3 lá muốn tạo phỏm
    VecByte tmpCards;
    K_VEC_INSERT(tmpCards, currentDealCards);
    for (size_t i = 0; i < cards.size(); i++) {
        K_VEC_REMOVE(tmpCards, cards.at(i));
    }
    
    // nếu 3 lá cards có chứa bài ăn thì bỏ lá đó ra khỏi bài ăn
    VecByte tmpEatCards;
    K_VEC_INSERT(tmpEatCards, currentEatCards);
    for (size_t i = 0; i < cards.size(); i++) {
        K_VEC_REMOVE(tmpEatCards, cards.at(i));
    }
    
    CardUtils::sortByValue_CRChB(true, tmpCards);
    CardUtils::sortByValue_CRChB(true, tmpEatCards);
    
    std::vector<Phom> tmp_phom = findPhoms(tmpCards, tmpEatCards, newFreeCards, isAddMorePhom);
    K_VEC_INSERT(newPhoms, tmp_phom);
    // add phỏm muốn tạo vô
    Phom phom(phomType, cards);
    newPhoms.push_back(phom);
    
    //Log.log(TAG_MAKE_PHOM, "phỏm sau khi tạo : " + newPhoms.toString());
    //Log.log(TAG_MAKE_PHOM, "bài ăn : " + currentEatCards.toString());
    //Log.log(TAG_MAKE_PHOM,
    // "bài rác sau khi tạo : " + newFreeCards.toString());
    
    for (size_t j = 0; j < currentEatCards.size(); j++) {
        //if (isContains(newFreeCards, currentEatCards.get(j))) {
        if(K_VEC_CONTAIN(newFreeCards, currentEatCards.at(j))) {
            //Log.error(
            // TAG_MAKE_PHOM,
            // "không thể tạo còn bài ăn "
            // + Card.card(currentEatCards.get(j), TYPECARD));
            return false;
        }
    }
    //Log.log(TAG_MAKE_PHOM, "có thể tạo phỏm này");
    return true;
}

bool PhomUtils::canFire(ubyte cardID,
                        VecByte dealCards, VecByte eatCards,
                        VecByte& myFreeCard, std::vector<Phom>& newPhoms,
                        VecByte& newFreeCards) {
    //Log.log(TAG_FIRE, "muốn đánh:" + Card.card(cardID, TYPECARD));
    //if (eatCards.contains(cardID, true)) {
    if(K_VEC_CONTAIN(eatCards, cardID)) {
        //Log.error(TAG_FIRE, "không được đánh bài ăn");
        return false;
    }
    
    if(K_VEC_CONTAIN(myFreeCard, cardID)){
        // nam trong bai rac thi khong can check nua vi khong co pha phom
        // nao
        return true;
    }
    
    //myFreeCard.clear();
    newPhoms.clear();
    newFreeCards.clear();
    
    // xóa lá bài đánh ra khỏi danh sách bài
    VecByte tmpCards;
    K_VEC_INSERT(tmpCards, dealCards);
    K_VEC_REMOVE(tmpCards, cardID);
    
    CardUtils::sortByValue_CRChB(true, tmpCards);
    std::vector<Phom> tmp_phom_array = findPhoms(tmpCards, eatCards, newFreeCards, true);
    K_VEC_INSERT(newPhoms, tmp_phom_array);
    
    //Log.log(TAG_FIRE, "phỏm sau khi đánh : " + newPhoms.toString());
    //Log.log(TAG_FIRE,
    //"bài rác sau khi đánh : " + Card.cards(newFreeCards, TYPECARD));
    
    for (size_t j = 0; j < eatCards.size(); j++) {
        if(K_VEC_CONTAIN(newFreeCards, eatCards.at(j))){
            //Log.error(
            //		TAG_FIRE,
            //		"không thể đánh còn bài ăn "
            //				+ Card.card(eatCards.get(j), TYPECARD));
            return false;
        }
    }
    //Log.log(TAG_FIRE, "có thể đánh");
    return true;
}

int PhomUtils::getPhomType(VecByte cards3) {
    VecByte tmp = cards3;
    CardUtils::sortByValue_CRChB(true, tmp);
    // Log.log("getPhomType", Card.cards(tmp, TYPECARD));
    bool ok = true;
    // check xem có phải phỏm 3 lá ko
    for (int i = 0; i < tmp.size() - 1; i++) {
        if (!isSameValue(tmp.at(i), tmp.at(i + 1))) {
            ok = false;
            break;
        }
    }
    if (ok)
        return PHOM_3;
    // ko phải phỏm 3 lá,check xem có phải phỏm sảnh ko
    ok = true;
    for (int i = 1; i < tmp.size(); i++) {
        if (!isStraightAndSameType(tmp.at(0), tmp.at(i), i)) {
            ok = false;
            break;
        }
    }
    if (ok)
        return PHOM_SANH;
    return NONE;
}

/**
 * tìm phỏm ko có bài ăn
 *
 * @param dealcards
 * @param freeCards
 * @return
 */
std::vector<Phom> PhomUtils::findPhom_( VecByte dealcards, VecByte& freeCards) {
    std::vector<Phom> phoms;
    freeCards.clear();
    //freeCards.insert(freeCards.begin(), dealcards.begin(), dealcards.end());
    K_VEC_INSERT(freeCards, dealcards);
    CardUtils::sortByValue_CRChB(true, freeCards);
    VecByte tmpDealCards;
    //tmpDealCards.insert(tmpDealCards.begin(), freeCards.begin(), freeCards.end());
    K_VEC_INSERT(tmpDealCards, freeCards);
    VecByte tmp3;
    int phomType;
    Phom phom;
    int i0, i1, i2;
    for (i0 = 0; i0 < (int)tmpDealCards.size() - 2; i0++) {
        tmp3.clear();
        if (tmpDealCards.at(i0) > -1) {
            tmp3.push_back(tmpDealCards.at(i0));
            for (i1 = i0 + 1; i1 < tmpDealCards.size() - 1; i1++) {
                if (tmpDealCards.at(i1) > -1) {
                    tmp3.push_back(tmpDealCards.at(i1));
                    for (i2 = i1 + 1; i2 < tmpDealCards.size(); i2++) {
                        if (tmpDealCards.at(i2) > -1) {
                            tmp3.push_back(tmpDealCards.at(i2));
                            phomType = getPhomType(tmp3);
                            if (phomType != NONE) {
                                // ket thuc khu vuc add them
                                Phom phom_tmp(phomType, tmp3);
                                CardUtils::sortByValue_CRChB(true,
                                                             phom_tmp._cards);
                                phoms.push_back(phom_tmp);
//                                auto it = std::find(freeCards.begin(), freeCards.end(), tmp3.at(0));
//                                if (it != freeCards.end())
//                                    freeCards.erase(it);
//                                it = std::find(freeCards.begin(), freeCards.end(), tmp3.at(1));
//                                if (it != freeCards.end())
//                                    freeCards.erase(it);
//                                it = std::find(freeCards.begin(), freeCards.end(), tmp3.at(2));
//                                if (it != freeCards.end())
//                                    freeCards.erase(it);
                                K_VEC_REMOVE(freeCards, tmp3.at(0));
                                K_VEC_REMOVE(freeCards, tmp3.at(1));
                                K_VEC_REMOVE(freeCards, tmp3.at(2));
                                
                                tmpDealCards[i0] = -1;
                                tmpDealCards[i1] = -1;
                                tmpDealCards[i2] = -1;
                                /*Log.log(TAG_FIND_PHOM, "tìm đc phỏm : "
                                 + phom.toString());*/
                                break;
                            }
                            else
                                tmp3.pop_back();
                        }
                    }
                    if (i2 < tmpDealCards.size()) {
                        // tìm thấy phỏm tại đây
                        break;
                    }
                    else
                        tmp3.pop_back();
                }
            }
        }
    }
    return phoms;
}


///**
//* tìm phỏm
//*
//* @param dealcards
//*            : bài tự có + bài rút
//* @param eatCards
//*            : bài ăn
//* @param freeCards
//*            : trả về bài rác sau khi tìm
//* @return : danh sách phỏm tạo được
//*/
std::vector<Phom> PhomUtils::findPhoms(VecByte dealcards, VecByte eatCards, VecByte& freeCards, bool isAddMoreCardIntoPhom) {
	/**
	* xếp các danh sách bài theo thứ tự điểm tăng dần
	*
	* check lần lượt từng bài ăn ,tìm phỏm tương ứng với nó, nếu có 1 con
	* bài ăn ko tạo dc phỏm nào thì dừng liền vì có lỗi . tìm hết phỏm ăn
	* ,tìm tiếp phỏm trong bài rác còn lại
	*/
	std::vector<Phom> phoms;
	freeCards.clear();
	//freeCards.insert(freeCards.begin(), dealcards.begin(), dealcards.end());
    K_VEC_INSERT(freeCards, dealcards);

	CardUtils::sortByValue_CRChB(true, freeCards);
	VecByte tmpDealCards;
	//tmpDealCards.insert(tmpDealCards.begin(), freeCards.begin(), freeCards.end());
    K_VEC_INSERT(tmpDealCards, freeCards);

	VecByte tmpEatCards;
	//tmpEatCards.insert(tmpEatCards.begin(), eatCards.begin(), eatCards.end());
    K_VEC_INSERT(tmpEatCards, eatCards);
	CardUtils::sortByValue_CRChB(true, tmpEatCards);

	VecByte tmp3;
	int phomType;
	Phom phom;
	 //khi card an bang hai phai check lai cac phom de tao ra phom hop ly
	 //nhat
	bool find2Phom = false;
	std::vector<Phom> phomsmartEat;
	int sumPhom = 0;
	/* tim tat ca cac phom 3 co the tao voi card an
	 neu 2 phom khong chua card trung voi nhau thi thuc hien tim kiem
	 chi add phom co nhieu gia tri nhat*/
	if (tmpEatCards.size() == 2) {
		std::vector<Phom> max_phom_1 = findMaxPhom3WithOneCard(tmpEatCards.at(0), tmpDealCards);
		for (size_t i = 0; i < max_phom_1.size(); i++) {
			std::vector<Phom> max_phom_2 = findMaxPhom3WithOneCard(tmpEatCards.at(1), tmpDealCards);
			for (size_t j = 0; j < max_phom_2.size(); j++) {
				if (!checkPhomHaveSameCard( max_phom_1.at(i), max_phom_2.at(j))) {
					if (sumValuePhom(max_phom_1.at(i)) + sumValuePhom(max_phom_2.at(j)) > sumPhom) {
						if (!phomsmartEat.empty())
							phomsmartEat.clear();
						//Log.log("Phom", "phomsmartEat " + phomsmartEat);
						phomsmartEat.push_back(max_phom_1.at(i));
						phomsmartEat.push_back(max_phom_2.at(j));
						find2Phom = true;
						sumPhom = sumValuePhom(max_phom_1.at(i)) + sumValuePhom(max_phom_2.at(j));
					}
				}
			}
		}
	}
	 //khi co 3 con bai an 
	if (tmpEatCards.size() == 3)
	{
		std::vector<Phom> max_phom_1 = findMaxPhom3WithOneCard(tmpEatCards.at(0), tmpDealCards);
		std::vector<Phom> max_phom_2 = findMaxPhom3WithOneCard(tmpEatCards.at(1), tmpDealCards);
		std::vector<Phom> max_phom_3 = findMaxPhom3WithOneCard(tmpEatCards.at(2), tmpDealCards);
		for (size_t i = 0; i < max_phom_1.size(); i++) {
			for (size_t j = 0; j < max_phom_2.size(); j++) {
				for (size_t k = 0; k < max_phom_3.size(); k++)
				{
					if (!checkPhomHaveSameCard(max_phom_1.at(i),
						max_phom_2.at(j),
						max_phom_3.at(k))) {

						if (sumValuePhom(max_phom_1.at(i))
							+ sumValuePhom(max_phom_2.at(j))
							+ sumValuePhom(max_phom_3.at(k)) > sumPhom) {
							if (!phomsmartEat.empty())
								phomsmartEat.clear();
							//Log.log("Phom", "phomsmartEat " + phomsmartEat);
							phomsmartEat.push_back(max_phom_1.at(i));
							phomsmartEat.push_back(max_phom_2.at(j));
							phomsmartEat.push_back(max_phom_3.at(k));
							find2Phom = true;
							sumPhom = sumValuePhom(max_phom_1.at(i))
								+ sumValuePhom(max_phom_2.at(j))
								+ sumValuePhom(max_phom_3.at(k));
						}
					}
				}

			}
		}
	}

	if (!find2Phom) {
		int i0, i1;
		for (int ie = 0; ie < tmpEatCards.size(); ie++) {
			tmp3.clear();
			tmp3.push_back(tmpEatCards.at(ie));
			for (i0 = 0; i0 < (int)tmpDealCards.size() - 1; i0++) {
				if ((int)tmpDealCards.at(i0) > -1) {
					tmp3.push_back(tmpDealCards.at(i0));
					for (i1 = i0 + 1; i1 < tmpDealCards.size(); i1++) {
						if ((int)tmpDealCards.at(i1) > -1) {
							tmp3.push_back(tmpDealCards.at(i1));
							phomType = getPhomType(tmp3);
							if (phomType != NONE) {
								 //ket thuc khu vuc add them
								Phom phom_tmp(phomType, tmp3);
								CardUtils::sortByValue_CRChB(true,
									phom_tmp._cards);
								phoms.push_back(phom_tmp);

								K_VEC_REMOVE(freeCards, tmp3.at(0));
								K_VEC_REMOVE(freeCards, tmp3.at(1));
								K_VEC_REMOVE(freeCards, tmp3.at(2));

								tmpDealCards[i0] = -1;
								tmpDealCards[i1] = -1;
								tmpEatCards[ie] = -1;
								//Log.log(TAG_FIND_PHOM, "tìm đc phỏm : "
								//	+ phom.toString());
								break;
							}
							else
								tmp3.pop_back();
						}
					}
					if (i1 < tmpDealCards.size()) {
						 //tìm thấy phỏm tại đây
						break;
					}
					else
						tmp3.pop_back();
				}
			}
		}
	}
	else {
		for (size_t i = 0; i < phomsmartEat.size(); i++) {
			for (size_t j = 0; j < phomsmartEat.at(i)._cards.size(); j++) {
//				auto it = std::find(freeCards.begin(), freeCards.end(), phomsmartEat.at(i)._cards.at(j));
//				if (it != freeCards.end())
//					freeCards.erase(it);
                K_VEC_REMOVE(freeCards, phomsmartEat.at(i)._cards.at(j));
			}
		}
		
		for (size_t i = 0;i<tmpEatCards.size();i++)
		{
			tmpEatCards[i] = -1;
		}
		//phoms.insert(phoms.end(), phomsmartEat.begin(), phomsmartEat.end());
        K_VEC_INSERT(phoms, phomsmartEat);
	}

	VecByte tmpCards;
	//tmpCards.insert(tmpCards.begin(), freeCards.begin(), freeCards.end());
    K_VEC_INSERT(tmpCards, freeCards);
	std::vector<Phom> tmpPhom = findPhom_(tmpCards, freeCards);
	//phoms.insert(phoms.end(), tmpPhom.begin(), tmpPhom.end());
    K_VEC_INSERT(phoms, tmpPhom);
//	auto it = std::find(tmpEatCards.begin(), tmpEatCards.end(), -1);
//	while (it != tmpEatCards.end()) {
//		tmpEatCards.erase(it);
//	}
    while (K_VEC_CONTAIN(tmpEatCards, 255))
    {
        K_VEC_REMOVE(tmpEatCards, 255);
    }
    
	if (tmpEatCards.size() > 0)
		//freeCards.insert(freeCards.end(), tmpEatCards.begin(), tmpEatCards.end());
        K_VEC_INSERT(freeCards, tmpEatCards);
	if (isAddMoreCardIntoPhom)
		addMoreCardIntoPhom(phoms, freeCards);

	return phoms;
}

/**
 * add them card free vao phom dang co tao thanh phom dai hon
 *
 * */
bool compareByte(int i, int j) { return (i < j); }
void PhomUtils::addMoreCardIntoPhom(std::vector<Phom>& phoms, VecByte& freeCard) {
    for (int i = 0; i < (int)phoms.size(); i++) {
        bool canAddCardToPhom = false;
        do {
            canAddCardToPhom = false;
            VecByte cardCanAdd = findCardCanAddPhom(phoms.at(i)._type,
                                                    phoms.at(i)._cards);
            for (int j = 0; j < (int)cardCanAdd.size(); j++) {
                auto it = std::find(freeCard.begin(), freeCard.end(), cardCanAdd.at(j));
                if (it != freeCard.end()) {
                    canAddCardToPhom = true;
                    phoms.at(i)._cards.push_back(cardCanAdd.at(j));
                    freeCard.erase(it);
                }
            }
        } while (canAddCardToPhom);
        std::sort(phoms.at(i)._cards.begin(), phoms.at(i)._cards.end(), compareByte);
    }
}

/**
 * tim card co the add them vao phom hien tai
 *
 * */
VecByte PhomUtils::findCardCanAddPhom(int typePhom, VecByte cards) {
    std::sort(cards.begin(), cards.end(), compareByte);
    VecByte cardResult;
    if (typePhom == PHOM_3)
        if (cards.size() == 4)
            return cardResult;
        else {
            if (cards.size() == 3) {
                cardResult.push_back((ubyte)((16 * (cards.at(0) / 4)) + 6
                                             - cards.at(0) - cards.at(1) - cards.at(2)));
                return cardResult;
            }
        }
    if (typePhom == PHOM_SANH) {
        if (cards.size() >= 3) {
            if (cards.at(0) / 4 - 1 >= 0) {
                cardResult.push_back((ubyte)((cards.at(0) / 4 - 1) * 4 + cards
                                             .at(0) % 4));
            }
            if (cards.at(cards.size() - 1) / 4 + 1 <= 12) {
                cardResult
                .push_back((ubyte)((cards.at(cards.size() - 1) / 4 + 1) * 4 + cards
                                   .at(cards.size() - 1) % 4));
            }
        }
    }
    
    return cardResult;
}

std::vector<Phom> PhomUtils::findPhomWithOneCard(ubyte cardID, VecByte freeCard)
{
    std::vector<Phom> phoms;
    VecByte temCardPhom;
    temCardPhom.push_back(cardID);
    bool canadd = false;
    // tim phom sanh
    // thuan toan co ban tim cac con co the add vao 2 dau cua mang phom
    do {
        canadd = false;
        std::sort(temCardPhom.begin(), temCardPhom.end(), compareByte);
        
        ubyte card = temCardPhom.at(0) - 4;
        if (card > 0 && K_VEC_CONTAIN(freeCard, card))
        {
            if (!K_VEC_CONTAIN(temCardPhom, card))
            {
                temCardPhom.push_back(card);
                canadd = true;
            }
        }
        std::sort(temCardPhom.begin(), temCardPhom.end(), compareByte);
        
        card = temCardPhom.at(temCardPhom.size() - 1) + 4;
        if (card < 52 && K_VEC_CONTAIN(freeCard, card))
        {
            if(!K_VEC_CONTAIN(temCardPhom, card))
            {
                temCardPhom.push_back(card);
                canadd = true;
            }
        }
        std::sort(temCardPhom.begin(), temCardPhom.end(), compareByte);
    }while (canadd);
    
    if (temCardPhom.size() > 2)
    {
        Phom phom(PHOM_SANH, temCardPhom);
        phoms.push_back(phom);
    }
    //tim phom 3cay
    temCardPhom.clear();
    temCardPhom.push_back(cardID);
    for (ubyte i = 0; i < 4; i++)
    {
        ubyte temp = cardID / 4;
        if(K_VEC_CONTAIN(freeCard, temp * 4 + i))
        {
            temCardPhom.push_back(temp * 4 + i);
        }
    }
    if (temCardPhom.size() > 2)
    {
        Phom phom(PHOM_3, temCardPhom);
        phoms.push_back(phom);
    }
        
    return phoms;
}

VecByte PhomUtils::canCanMakePhomOneCard(ubyte cardID)
{
    VecByte cards;
    if (cardID - 4 >= 0 && cardID - 4 <= 51)
    {
        cards.push_back(cardID - 4);
    }
    if (cardID + 4 >= 0 && cardID + 4 <= 51)
    {
        cards.push_back(cardID - 4);
    }
    
    return cards;
}

///**
//* tim phom tat ca phom 3 co the tao duoc voi card do
//* */
std::vector<Phom> PhomUtils::findMaxPhom3WithOneCard(ubyte cardID, VecByte freeCard) {
	std::vector<Phom> phoms;
	VecByte temCardPhom;
//	if (std::find(freeCard.begin(), freeCard.end(), cardID + 4) != freeCard.end()
//		&& std::find(freeCard.begin(), freeCard.end(), cardID + 8) != freeCard.end() ) {
    if (K_VEC_CONTAIN(freeCard, cardID + 4)
        && K_VEC_CONTAIN(freeCard, cardID + 8)) {
		temCardPhom.push_back(cardID);
		temCardPhom.push_back((ubyte)(cardID + 4));
		temCardPhom.push_back((ubyte)(cardID + 8));
		Phom phom(PHOM_SANH, temCardPhom);
        bool isExist = false;
        for (auto it : phoms)
        {
            if(it._type == phom._type && it._cards == phom._cards)
            {
                isExist = true;
                break;
            }
        }
        if(!isExist)
        {
            phoms.push_back(phom);
        }
        
		temCardPhom.clear();
	}

//	if(std::find(freeCard.begin(), freeCard.end(), cardID - 4) != freeCard.end() &&
//		std::find(freeCard.begin(), freeCard.end(), cardID + 4) != freeCard.end() ) {
    if (K_VEC_CONTAIN(freeCard, cardID - 4)
        && K_VEC_CONTAIN(freeCard, cardID + 4)) {
		temCardPhom.push_back(cardID);
		temCardPhom.push_back((ubyte)(cardID - 4));
		temCardPhom.push_back((ubyte)(cardID + 4));
		Phom phom(PHOM_SANH, temCardPhom);
        bool isExist = false;
        for (auto it : phoms)
        {
            if(it._type == phom._type && it._cards == phom._cards)
            {
                isExist = true;
                break;
            }
        }
        if(!isExist)
        {
            phoms.push_back(phom);
        }
		temCardPhom.clear();
	}

//	if (std::find(freeCard.begin(), freeCard.end(), cardID - 4) != freeCard.end() &&
//		std::find(freeCard.begin(), freeCard.end(), cardID - 8) != freeCard.end()) {
    if (K_VEC_CONTAIN(freeCard, cardID - 4)
        && K_VEC_CONTAIN(freeCard, cardID - 8)) {
		temCardPhom.push_back(cardID);
		temCardPhom.push_back((ubyte)(cardID - 4));
		temCardPhom.push_back((ubyte)(cardID - 8));
		Phom phom(PHOM_SANH, temCardPhom);
        bool isExist = false;
        for (auto it : phoms)
        {
            if(it._type == phom._type && it._cards == phom._cards)
            {
                isExist = true;
                break;
            }
        }
        if(!isExist)
        {
            phoms.push_back(phom);
        }
		temCardPhom.clear();
	}

	 //tim phom 3 cay
	temCardPhom.clear();
	temCardPhom.push_back(cardID);
	for (auto i = 0; i < 4; i++) {
		ubyte temp = (ubyte)(cardID / 4);
//		if(std::find(freeCard.begin(), freeCard.end(), temp*4 + i) != freeCard.end()) {
        if (K_VEC_CONTAIN(freeCard, temp * 4 + i)) {
			temCardPhom.push_back((ubyte)(temp * 4 + i));
		}
	}
	if (temCardPhom.size() == 3) {
		Phom phom(PHOM_3, temCardPhom);
		phoms.push_back(phom);
	}
	else {
		if (temCardPhom.size() > 3) {
			temCardPhom.clear();
			temCardPhom.push_back((ubyte)((cardID / 4) * 4 + 0));
			temCardPhom.push_back((ubyte)((cardID / 4) * 4 + 1));
			temCardPhom.push_back((ubyte)((cardID / 4) * 4 + 2));
			Phom phom(PHOM_3, temCardPhom);
			phoms.push_back(phom);

			temCardPhom.clear();
			temCardPhom.push_back((ubyte)((cardID / 4) * 4 + 0));
			temCardPhom.push_back((ubyte)((cardID / 4) * 4 + 1));
			temCardPhom.push_back((ubyte)((cardID / 4) * 4 + 3));
			Phom phom1(PHOM_3, temCardPhom);
			phoms.push_back(phom1);

			temCardPhom.clear();
			temCardPhom.push_back((ubyte)((cardID / 4) * 4 + 0));
			temCardPhom.push_back((ubyte)((cardID / 4) * 4 + 2));
			temCardPhom.push_back((ubyte)((cardID / 4) * 4 + 3));
			Phom phom2(PHOM_3, temCardPhom);
			phoms.push_back(phom2);

			temCardPhom.clear();
			temCardPhom.push_back((ubyte)((cardID / 4) * 4 + 1));
			temCardPhom.push_back((ubyte)((cardID / 4) * 4 + 2));
			temCardPhom.push_back((ubyte)((cardID / 4) * 4 + 3));
			Phom phom3(PHOM_3, temCardPhom);
			phoms.push_back(phom3);
		}
	}
	return phoms;
}

/**
* kiem tra xem card trong phom co trung nhau khong co là true khong là
* false
**/
bool PhomUtils::checkPhomHaveSameCard(Phom phom1, Phom phom2) {
	for (size_t i = 0; i < phom1._cards.size(); i++)
	{
		for (size_t j = 0; j < phom2._cards.size(); j++)
		{
			if(phom1._cards[i] == phom2._cards[j])
				return true;
		}
	}
	return false;
}

/**
* kiem tra xem card trong phom co trung nhau khong co là true khong là
* false
* */
bool PhomUtils::checkPhomHaveSameCard(Phom phom1, Phom phom2, Phom phom3) {
	if (checkPhomHaveSameCard(phom1, phom2)) return checkPhomHaveSameCard(phom1, phom2);
	if (checkPhomHaveSameCard(phom1, phom3)) return checkPhomHaveSameCard(phom1, phom3);
	if (checkPhomHaveSameCard(phom2, phom3)) return checkPhomHaveSameCard(phom2, phom3);

	return false;
}

int PhomUtils::sumValuePhom(Phom phom) {
	int sum = 0;
	for (size_t i = 0; i < phom._cards.size(); i++) {
		sum += phom._cards.at(i);
	}
	return sum;
}

std::vector<Phom> PhomUtils::checkPhom(VecByte eatCards, VecByte dealCards)
{
    bool find2Phom = false;
    std::vector<Phom> phomsmartEat;
    int sumPhom = 0;
    std::vector<Phom> max_phom_1 = findMaxPhom3WithOneCard(eatCards.at(0), dealCards);
    std::vector<Phom> max_phom_2 = findMaxPhom3WithOneCard(eatCards.at(1), dealCards);
    for (int i = 0; i < max_phom_1.size(); i++)
    {
        for (int j = 0; j < max_phom_2.size(); j++)
        {
            if(!checkPhomHaveSameCard(max_phom_1.at(i), max_phom_2.at(j)))
            {
                if (sumValuePhom(max_phom_1.at(i)) + sumValuePhom(max_phom_2.at(j)) > sumPhom)
                {
                    phomsmartEat.clear();
                    phomsmartEat.push_back(max_phom_1.at(i));
                    phomsmartEat.push_back(max_phom_2.at(j));
                    find2Phom = true;
                    sumPhom = sumValuePhom(max_phom_1.at(i)) + sumValuePhom(max_phom_2.at(j));
                }
            }
        }
    }
    
    return phomsmartEat;
}

std::vector<Phom> PhomUtils::findPhomFromListCard(VecByte cards, VecByte newFree)
{
    VecByte eatCards;
    std::vector<Phom> phoms = findPhoms(cards, eatCards, newFree, true);
    
    return phoms;
}







