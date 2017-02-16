package parse;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.lang.reflect.Array;
import java.lang.reflect.Field;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import java.net.URL;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.sql.Date;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;

import javax.lang.model.type.TypeVariable;

public class Utils {

	private static Class[] getClasses(String packageName) throws ClassNotFoundException, IOException {
		ClassLoader classLoader = Thread.currentThread().getContextClassLoader();
		assert classLoader != null;
		String path = packageName.replace('.', '/');
		Enumeration<URL> resources = classLoader.getResources(path);
		List<File> dirs = new ArrayList<File>();
		while (resources.hasMoreElements()) {
			URL resource = resources.nextElement();
			dirs.add(new File(resource.getFile()));
		}
		ArrayList<Class> classes = new ArrayList<Class>();
		for (File directory : dirs) {
			classes.addAll(findClasses(directory, packageName));
		}
		return classes.toArray(new Class[classes.size()]);
	}

	public static List<Class> findClasses(File directory, String packageName) throws ClassNotFoundException {
		List<Class> classes = new ArrayList<Class>();
		if (!directory.exists()) {
			return classes;
		}
		File[] files = directory.listFiles();
		for (File file : files) {
			if (file.isDirectory()) {
				assert !file.getName().contains(".");
				classes.addAll(findClasses(file, packageName + "." + file.getName()));
			} else if (file.getName().endsWith(".class")) {
				classes.add(
						Class.forName(packageName + '.' + file.getName().substring(0, file.getName().length() - 6)));
			}
		}
		return classes;
	}
	
	public static void WriteFile(String text,String fileName) throws IOException {

		BufferedWriter output = null;
		try {
			File file = new File(fileName);
			
			output = new BufferedWriter(new FileWriter(file));
			output.write(text);
		} catch (IOException e) {
			e.printStackTrace();
		} finally {
			if (output != null) {
				output.close();
			}
		}
	}
	public static String ReadFile(String filename) throws FileNotFoundException, IOException
	{
		try(BufferedReader br = new BufferedReader(new FileReader(filename))) {
		    StringBuilder sb = new StringBuilder();
		    String line = br.readLine();

		    while (line != null) {
		        sb.append(line);
		        sb.append(System.lineSeparator());
		        line = br.readLine();
		    }
		    String everything = sb.toString();
		    return everything;
		}
		
	}
	public static String getValue(Field f) throws IllegalArgumentException, IllegalAccessException
	{
		Class<?> t = f.getType();
		if(t == int.class){
			return f.getInt(null)+"";
		}else if(t == double.class){
		    return f.getDouble(null)+"";
		}else if(t == long.class){
		    return f.getLong(null)+"";
		}else if(t == float.class){
		    return f.getFloat(null)+"";
		}
		else if(t == String.class){
			 return "\""+f.get(null)+"\"";
		}
		else if(t == byte.class||t == Byte.class){
			 return +f.getByte(null)+"";
		}
		return "";
	}
	
	public static Package GetPackgeTypeInclude(Field f) {
		 if(f.getType() == ArrayList.class ||f.getType() == List.class||f.getType() ==Array.class)
		 {
			  Class type = getTypeArrayList(f);
			 return type.getPackage();
		 }
		return f.getType().getPackage();
		   
	}
	public static String GetFieldTypeInclude(Field f) {
		 if(f.getType() == ArrayList.class ||f.getType() == List.class||f.getType() ==Array.class)
		 {
			  Class type = getTypeArrayList(f);
			 return GetFieldType(type);
		 }
		return (GetFieldType(f.getType()));
		   
	}
	public static String GetFieldType(Field f) {
		 if(f.getType() == ArrayList.class||f.getType() == List.class||f.getType() ==Array.class)
		 {
			 String className = "std::vector<%s>";
			  Class type = getTypeArrayList(f);
			 String typeName =GetFieldType(type);
			 return String.format(className, new String[]{typeName});
		 }
		return (GetFieldType(f.getType()));
		   
	}
	public static String GetFieldType(Class actualType) {
		 
		//Pair<String, String>
			String className ="";
		   if (actualType == String.class)
		   {
			   className = "std::string";
		   }else if (actualType == Integer.class||actualType == int.class)
		   {
			   className = "int";
		   }else if (actualType == Boolean.class ||actualType ==boolean.class)
		   {
			   className = "bool";
		   }else if (actualType == Float.class)
		   {
			   className = "double";
		   }else if (actualType == Long.class||actualType == long.class)
		   {
			   className = "int64_t";
		   }else if (actualType == Double.class)
		   {
			   className = "double";
		   }else if (actualType == Short.class)
		   {
			   className = "int";
		   }else if (actualType == Byte.class)
		   {
			   className = "int";
		   }else if (actualType == Character.class)
		   {
			   className = "char";
		   }else if (actualType == Date.class)
		   {
			   className = "time_t";
		   }else if (actualType == Enum.class)
		   {
			   className = "std::string";
		   }
		   else
		   {
			   
			   className = actualType.getSimpleName();
		   }
		   return className;
		   
	}
	public static Boolean GetFieldTypeInclue(Field f) {
		 if(f.getType() == ArrayList.class ||f.getType() == List.class||f.getType() ==Array.class)
		 {
			  Class type = getTypeArrayList(f);
			 return GetFieldTypeInclue(type);
		 }
		  return GetFieldTypeInclue(f.getType());
		   
	}
	public static Boolean GetFieldTypeInclue(Class actualType) {
		 //Class actualType = field.getClass();
		//Pair<String, String>
			
		   if (actualType == String.class)
		   {
			   return false;
		   }else if (actualType == Integer.class||actualType == int.class)
		   {
			   return false;
		   }else if (actualType == Boolean.class||actualType ==boolean.class)
		   {
			   return false;
		   }else if (actualType == Float.class||actualType == float.class)
		   {
			   return false;
		   }else if (actualType == Long.class||actualType == long.class)
		   {
			   return false;
		   }else if (actualType == Double.class ||actualType == double.class)
		   {
			   return false;
		   }else if (actualType == Short.class||actualType == short.class)
		   {
			   return false;
		   }else if (actualType == Byte.class||actualType == byte.class)
		   {
			   return false;
		   }else if (actualType == Character.class)
		   {
			   return false;
		   }else if (actualType == Date.class)
		   {
			   return false;
		   }
		   return true;
		   
	}
	public static boolean containsParagemWithName(ArrayList<String> paragems, String name) {
	    for (String p : paragems) {
	        if (p.equals(name)) {
	            return true;
	        }
	    }
	    return false;
	}
	public static String getInclude(ArrayList<String> listInclude)
	{
		String str_include ="";
		for (String str : listInclude) {
			str_include+=str+System.lineSeparator();
		}
		return str_include; 
	}
	public static  Class getTypeArrayList(Field field)
	{
//			if(field.getType() != ArrayList.class ||field.getType() != List.class)
//			{
//				return null;
//			}
	        ParameterizedType ptype = (ParameterizedType) field.getGenericType();
	        // list the actual type arguments
	        Type[] targs = ptype.getActualTypeArguments();
	   
	        for (int j = 0; j < targs.length; j++) {
	            Class tclas = (Class) targs[j];
	            System.out.println(tclas.getName());
	            return tclas;
	        }
	        return null;
	}
	
	public static void createFolder(String path)
	{
		Path newDirectoryPath = Paths.get(path);
		 
		if (!Files.exists(newDirectoryPath)) {
		    try {
		        Files.createDirectory(newDirectoryPath);
		    } catch (IOException e) {
		        System.err.println(e);
		    }
		}
	}
	public static String decapitalize(String string){
		 char[] stringArray = string.toCharArray();
		    stringArray[0] = Character.toUpperCase(stringArray[0]);
		    return string = new String(stringArray);
	}
	
}
