package parse;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.ArrayList;

public class WriteEnum {
	private final static String STRING_CST = "static std::string %s;";
	public static void WriteFile(String path,Class clss) throws FileNotFoundException, IOException, IllegalArgumentException, IllegalAccessException {
		String textH = Utils.ReadFile(FileConstans.FORMAT_ENUM);
		String textCpp = Utils.ReadFile(FileConstans.FORMAT_ENUM_CPP);
		String content ="";
		String contentCpp ="";
		for(Object obj : clss.getEnumConstants())
		{
			content +=String.format(STRING_CST, new String[]{obj.toString(),obj.toString()})+System.lineSeparator();
			contentCpp += String.format(WriteFileH.FIELD_STATIC_CPP, new String[]{"std::string",clss.getSimpleName(),"\""+obj.toString(),obj.toString()+"\""})+System.lineSeparator();
		}
		textH = String.format(textH, new String[]{clss.getSimpleName(),content});
		textH =textH.replace(FormatConstans.CLASS_NAME, clss.getSimpleName());
		textCpp = textCpp.replace(FormatConstans.CLASS_NAME, clss.getSimpleName());
		textCpp =textCpp.replace(FormatConstans.STATIC_FIELD, contentCpp);
		Utils.WriteFile(textH, path+clss.getSimpleName()+".h");
		Utils.WriteFile(textCpp, path+clss.getSimpleName()+".cpp");

	}
}
