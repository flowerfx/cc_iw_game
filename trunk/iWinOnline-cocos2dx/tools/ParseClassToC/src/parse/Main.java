package parse;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.net.URL;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;

import iwin.vn.json.message.*;

public class Main {

	public static final String CLASS_FORMAT_H = "#include \"platform/CCPlatformMacros.h\"" + "\n" + "#include <string>"
			+ "\n" + "#include \"json/document.h\"" + "\n" + "namespace iwinmesage" + "\n" + "{" + "\n" + "class "
			+ "%s" + "{" + "\n" + "%s" + "\n" + "public:" + "\n" + "Avatar();" + "\n" + "virtual ~Avatar();" + "\n"
			+ "rapidjson::Document toJson();" + "\n" + "};" + "\n" + "}";
	public static final String CLASS_FORMAT_CPP = "";
	public static final String FIELD_ADD = "CC_SYNTHESIZE(\"%s\", \"%s\",\"%s\" );";
	public static final String OUTPUT_FOLDER = "OutPut/";

	public static void main(String[] args)
			throws ClassNotFoundException, IOException, IllegalArgumentException, IllegalAccessException {
		// TODO Auto-generated method stub
		try {
			FileConstans.LoadDataFormat();

			String packageParse = "iwin.vn.json.message";
			Class[] listClass = getClasses(packageParse);
			for (int i = 0; i < listClass.length; i++) {
				String folder = listClass[i].getPackage().getName().replace(packageParse, "").replace(".", "");
				String subFolder = OUTPUT_FOLDER + folder;
				if (subFolder.length() > 0) {
					subFolder += "/";
				}
				Utils.createFolder(subFolder);
				if (listClass[i].isEnum()) {
					WriteEnum.WriteFile(subFolder, listClass[i]);
					continue;
				}

				String strInclueStatic = WriteFileH.WriteFile(packageParse, subFolder, listClass[i]);
				WriteFileCpp.WriteFile(subFolder, listClass[i], strInclueStatic);

			}
			System.out.println("************Sucess!************");
		} catch (Exception ex) {
			System.out.println("_____Error :____");
			System.out.println(ex.toString());
		}
	}

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

	private static List<Class> findClasses(File directory, String packageName) throws ClassNotFoundException {
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

	private static void WriteFile(String text, String fileName) throws IOException {

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

}
