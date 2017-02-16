package org.cocos2dx.cpp.googlegameservices;

/**
 * thông tin account google plus
 * 
 * @author hungldm
 * 
 */
public class MGoogleAccount {

	public String id;
	public String name;
	public String given_name;
	public String link;
	public String picture;
	public String gender;
	public String locale;
	public String email;
	
	public MGoogleAccount() {
	}

	public MGoogleAccount(String json) {
//		GoogleAccount account = GameController.game.json.fromJson(GoogleAccount.class, json);
//		id = account.id;
//		name = account.name;
//		given_name = account.given_name;
//		link = account.link;
//		picture = account.picture;
//		gender = account.gender;
//		locale = account.locale;
//		email = account.email;
	}

	@Override
	public String toString() {
//		return Util.getLogString("GoogleAccount", "id", "name", "given_name",
//				"link", "picture", "gender", "locale", "email",
//				id, name, given_name, link, picture, gender,
//				locale, email);
		return "";
	}

}
