package parse;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.Map;

public class WriteFileH {
	public static final String INCLUDE = "#include \"%s.h\"";
	public static final String FIELD_CONST = "static  %s %s;";
	public static final String FIELD_STATIC = "";
	public static final String FIELD = "CC_SYNTHESIZE(%s, %s, %s);";
	public static final String FIELD_STATIC_CPP = "%s %s::%s = %s;";
	
	public static String WriteFile(String packageParse, String path,Class clss) throws FileNotFoundException, IOException, IllegalArgumentException, IllegalAccessException {
		String text = Utils.ReadFile(FileConstans.FORMAT_H);
		String fieldList=System.lineSeparator();
		
		ArrayList<String> listInclude = new ArrayList<>();
		Field fields[] = clss.getDeclaredFields();
		String staticField="";
		String staticFieldInclueCpp="";
		for (Field field :fields) {
			String textU="";
			String fieldType = Utils.GetFieldType(field);
			String fieldTypeInClude = Utils.GetFieldTypeInclude(field);
			
			if(Utils.GetFieldTypeInclue(field) )
			{

//				String folder = clss.getPackage().getName().replace(packageParse, "");
//				String fieldP = Utils.GetPackgeTypeInclude(field).getName().replace(packageParse, "");
//				
//				if(folder != null &&fieldP!= null ){
//					folder = folder.replace(".", "");
//					fieldP= fieldP.replace(".", "");
//					if(folder.equals(fieldP))
//					{
//						fieldTypeInClude ="../"+fieldP+"/"+fieldTypeInClude;
//					}
//					if(!listInclude.contains(String.format(INCLUDE, fieldTypeInClude))){
//						listInclude.add(String.format(INCLUDE, fieldTypeInClude));
//					}
//				}
				
				if(!listInclude.contains(String.format(INCLUDE, fieldTypeInClude))){
					listInclude.add(String.format(INCLUDE, fieldTypeInClude));
				}
			}
			if(java.lang.reflect.Modifier.isFinal(field.getModifiers())
					&&java.lang.reflect.Modifier.isStatic(field.getModifiers()))
			{
				if(fieldType.equals("std::string"))
				{ 
					///fieldType ="std::string";
				}else 	if(fieldType.equals("byte"))
				{
					fieldType ="int";
				}
				staticField+=String.format(FIELD_CONST, new String[]{fieldType,field.getName(),Utils.getValue(field) })+System.lineSeparator();
				staticFieldInclueCpp+=String.format(FIELD_STATIC_CPP, new String[]{fieldType,clss.getSimpleName(),field.getName(),Utils.getValue(field) })+System.lineSeparator();
				//textU =String.format(FIELD_CONST, new String[]{fieldType,field.getName(),Utils.getValue(field) });
				continue;
			}else{
			
				textU = String.format(FIELD, new String[]{fieldType,field.getName(),Utils.decapitalize(field.getName())});
			}
			fieldList+=textU+System.lineSeparator();
			
		}
		text =text.replace(FormatConstans.STATIC_FIELD, staticField);
		text =  text.replace(FormatConstans.INCLUDE, Utils.getInclude(listInclude));
		text =text.replace(FormatConstans.CLASS_NAME, clss.getSimpleName());
		text =text.replace(FormatConstans.CREATE_FIELD, fieldList);
		//text = String.format(text, new String[]{Utils.getInclude(listInclude),clss.getSimpleName(),fieldList,clss.getSimpleName(),clss.getSimpleName()});
		//System.out.println(text);
		
	
		Utils.WriteFile(text,path+clss.getSimpleName()+".h");
		return staticFieldInclueCpp;

	}
	
	
	
	
	
	
	
		
}

