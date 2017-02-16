package parse;

import java.io.FileNotFoundException;
import java.io.IOException;

import javax.swing.plaf.FileChooserUI;

public class FileConstans {
	public static final String FORMAT_H = "format/Format.h";
	public static final String FORMAT_CPP = "format/Format.cpp";
	public static final String FORMAT_ENUM = "format/enum.txt";
	public static final String FORMAT_ENUM_CPP = "format/enum_cpp.txt";
	//dinh nghia file cpp
	private static final String READ_FIELD_TO_JSON = "format/read_field_to_json.txt";
	private static final String READ_ARRAY_TO_JSON = "format/read_array_to_json.txt";
	private static final String WRIETE_JSON_TO_FIELD = "format/write_json_to_field.txt";
	private static final String WRIETE_JSON_TO_ARRAY = "format/write_json_to_array.txt";
	private static final String JSON_TO_ARRAY_PUSH = "format/json_to_aray_push.txt";
	private static final String JSON_TO_FIELD_INCLUDE = "format/json_to_field_include.txt";
	

	
	public static  String FIELD_TO_JSON = "";
	public static  String ARRAY_TO_JSON = "";
	public static  String JSON_TO_FILED = "";
	public static  String JSON_TO_ARRAY = "";
	public static  String JsonToArrayPush = "";
	public static  String JsonToFieldInclude = "";
	public static  String JormatEnumCpp = "";
	
	public static void LoadDataFormat() throws FileNotFoundException, IOException
	{
		FIELD_TO_JSON =Utils.ReadFile(READ_FIELD_TO_JSON);
		ARRAY_TO_JSON =Utils.ReadFile(READ_ARRAY_TO_JSON);
		JSON_TO_FILED =Utils.ReadFile(WRIETE_JSON_TO_FIELD);
		JSON_TO_ARRAY =Utils.ReadFile(WRIETE_JSON_TO_ARRAY);
		JsonToArrayPush =Utils.ReadFile(JSON_TO_ARRAY_PUSH);
		JsonToFieldInclude =Utils.ReadFile(JSON_TO_FIELD_INCLUDE);
		JormatEnumCpp =Utils.ReadFile(FORMAT_ENUM_CPP);
	}
}
