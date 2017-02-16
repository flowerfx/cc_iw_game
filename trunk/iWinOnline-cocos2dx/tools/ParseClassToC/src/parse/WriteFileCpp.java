package parse;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.lang.reflect.Array;
import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.List;

import javax.rmi.CORBA.Util;

public class WriteFileCpp {
//	public static final String FIELD_TO_JSON = "document.AddMember(\"%s\", %s, %s);";
//	public static final String ARRAY_TO_JSON = "rapidjson::Value arrays(rapidjson::kArrayType);"+System.lineSeparator()+
//		"for (size_t i = 0; i < list.size(); i++){"+System.lineSeparator()+"%s"+System.lineSeparator()+"}"+System.lineSeparator()+"document.AddMember(\"%s\",%s, %s);";
//	
	//public static final String JSON_TO_AIELD = "set%s(document[\"%s\"].%s());";
	
	public static void WriteFile(String path,Class clss,String includeStatic) throws FileNotFoundException, IOException, IllegalArgumentException, IllegalAccessException {
		String text = Utils.ReadFile(FileConstans.FORMAT_CPP);
		String readJson =setArrayToJson(clss)+ setMethodToJson(clss);
		String wrieteJson =getJsonToField(clss) + getJsonToArray(clss);
		text = text.replace(FormatConstans.STATIC_FIELD, includeStatic);
		text = text.replace(FormatConstans.CLASS_NAME, clss.getSimpleName());
		text = text.replace(FormatConstans.TO_JSON_CONTENT, readJson);
		text = text.replace(FormatConstans.TO_DATA_CONTENT, wrieteJson);
		Utils.WriteFile(text, path+clss.getSimpleName()+".cpp");

	}
	
	private static ArrayList<String> instanceClass(Class clss)
	{
		ArrayList<String> arrs = new ArrayList<>();
		for(int i=0;i<6;i++)
		{
			arrs.add(clss.getSimpleName());
		}
		
		return arrs;
	}
	private static String setMethodToJson(Class clss)
	{
		String data ="";
		Field fields[] = clss.getDeclaredFields();
		for (Field field : fields) {
			if((java.lang.reflect.Modifier.isFinal(field.getModifiers())
					&&java.lang.reflect.Modifier.isStatic(field.getModifiers())))
			{
				continue;
			}
			if(field.getType() != ArrayList.class && field.getType() != List.class && field.getType() !=Array.class){
				if(field.getType() == String.class)
				{
					String fstr = "v"+field.getName();
					data+=String.format("rapidjson::Value %s(%s.c_str(), %s.size());",new String[]{fstr,field.getName(),field.getName()})+System.lineSeparator();
					data+=String.format(FileConstans.FIELD_TO_JSON, new String[]{field.getName(),fstr});
				}else
				{
					String tojson ="";
					if(Utils.GetFieldTypeInclue(field))
					{
						tojson=".toJson()";
					}
					data+=String.format(FileConstans.FIELD_TO_JSON, new String[]{field.getName(),field.getName()+tojson});
				}
			}
			
		}
		return data;
	}
	private static String setArrayToJson(Class clss)
	{
		String data ="";
		Field fields[] = clss.getDeclaredFields();
		for (Field field : fields) {
			if((java.lang.reflect.Modifier.isFinal(field.getModifiers())
					&&java.lang.reflect.Modifier.isStatic(field.getModifiers())))
			{
				continue;
			}
			if(field.getType() == ArrayList.class || field.getType() == List.class||field.getType() ==Array.class){
			String arraT = getAddValueArray(field);
			//data+=String.format(ARRAY_TO_JSON, new String[]{arraT,field.getName(),field.getName(),field.getName()});
			data+=String.format(FileConstans.ARRAY_TO_JSON, new String[]{field.getName(),arraT,field.getName(),field.getName(),field.getName()});
			data = data.replace(FormatConstans.FIELD_TYPE, field.getName());
			}
		}
		return data;
	}
	
	private static String getJsonToField(Class clss)
	{
		String data ="";
		Field fields[] = clss.getDeclaredFields();
		for (Field field :fields) {
			if(field.getType() == List.class||field.getType() == ArrayList.class||field.getType() ==Array.class 
					|| (java.lang.reflect.Modifier.isFinal(field.getModifiers())
					&&java.lang.reflect.Modifier.isStatic(field.getModifiers())))
			{
				continue;
			}
			if(Utils.GetFieldTypeInclue(field))
			{
				String d = FileConstans.JsonToFieldInclude.replace(FormatConstans.FIELD_NAME, field.getName());
				String typeName =Utils.GetFieldType(field);
				d= d.replace(FormatConstans.FIELD_TYPE, typeName);
				data +=String.format(d, new String[]{field.getName(),Utils.decapitalize(field.getName())});
			}else
			{
				data+=String.format(FileConstans.JSON_TO_FILED, new String[]{field.getName(),Utils.decapitalize(field.getName()),field.getName(),getReadValue(field.getType())});
			}
			
		}
		return data;
	}
	
	private static String getJsonToArray(Class clss)
	{
		String data ="";
		Field fields[] = clss.getDeclaredFields();
		for (Field field : fields) {
			if( (java.lang.reflect.Modifier.isFinal(field.getModifiers())
					&&java.lang.reflect.Modifier.isStatic(field.getModifiers())))
			{
				continue;
			}
			if(field.getType() == ArrayList.class||field.getType() == List.class ||field.getType() ==Array.class){
			String typeRead = getReadValue(Utils.getTypeArrayList(field));
			String dataRead = "";
			if(typeRead.length() >1)
			{
				dataRead = String.format(field.getName()+".push_back(arr|field_name|[i].%s());", new String[]{typeRead});
			}else
			{
				 Class type =Utils.getTypeArrayList(field);
				 String typeName =Utils.GetFieldType(type);
				dataRead  = FileConstans.JsonToArrayPush.replace(FormatConstans.FIELD_TYPE,typeName);
				
			}
			dataRead  = dataRead.replace(FormatConstans.FIELD_NAME,field.getName());
			
			data+=String.format(FileConstans.JSON_TO_ARRAY.replace(FormatConstans.FIELD_NAME,field.getName()), new String[]{field.getName(),field.getName(),field.getName(),field.getName(),dataRead});
			}
			
		}
		return data;
	}
	
	private static String getAddValueArray(Field field)
	{
		String addT ="";
		Class actualType = Utils.getTypeArrayList(field);
		 if (actualType == String.class)
		   {
			 String stringPush = "rapidjson::Value().SetString(%s.at(i).c_str()," +field.getName()+".at(i).size())";
			 addT =String.format("|field_type|.PushBack(%s, locator);",new String[]{stringPush});
		   }else if (actualType == Integer.class||actualType == int.class)
		   {
			   addT = "|field_type|.PushBack(%s.at(i), locator);";
		   }else if (actualType == Boolean.class||actualType == boolean.class)
		   {
			   addT = "|field_type|.PushBack(%s.at(i), locator);";
		   }else if (actualType == Float.class||actualType == float.class)
		   {
			   addT = "|field_type|.PushBack(%s.at(i), locator);";
		   }else if (actualType == Long.class||actualType == long.class)
		   {
			   addT = "|field_type|.PushBack(%s.at(i), locator);";
		   }else if (actualType == Double.class||actualType == double.class)
		   {
			   addT = "|field_type|.PushBack(%s.at(i), locator);";
		   }else if (actualType == Short.class||actualType == short.class)
		   {
			   addT = "|field_type|.PushBack(%s.at(i), locator);";
		   }else if (actualType == Byte.class||actualType == byte.class)
		   {
			   addT = "|field_type|.PushBack(%s.at(i), locator);";
		   }else if (actualType == Character.class)
		   {
			   addT = "|field_type|.PushBack(rapidjson::Value().SetInt(%s.at(i)), locator);";
		   }
		   else
		   {
			   
			   addT = "|field_type|.PushBack(%s.at(i).toJson(), locator);";
		   }
		return String.format("arr"+addT, new String[]{field.getName()});
	}
	private static String getReadValue(Class actualType)
	{
		String addT ="";
		 if (actualType == String.class)
		   {
			 addT = "GetString";
		   }else if (actualType == Integer.class||actualType == int.class)
		   {
			   addT = "GetInt";
		   }else if (actualType == Boolean.class||actualType == boolean.class)
		   {
			   addT = "GetBool";
		   }else if (actualType == Float.class||actualType == float.class)
		   {
			   addT = "GetDouble";
		   }else if (actualType == Long.class||actualType == long.class)
		   {
			   addT = "GetInt64";
		   }else if (actualType == Double.class||actualType == double.class)
		   {
			   addT = "GetDouble";
		   }else if (actualType == Short.class||actualType == short.class)
		   {
			   addT = "GetInt";
		   }else if (actualType == Byte.class||actualType == byte.class)
		   {
			   addT = "GetInt";
		   }else if (actualType == Character.class)
		   {
			   addT = "GetInt";
		   }
		  
		return addT;
	}
	
}
