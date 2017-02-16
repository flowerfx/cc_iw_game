import xml.sax




class VisualParser( xml.sax.ContentHandler ):
   def __init__(self):
      self.CurrentData = ""
      self.inTagItemGroup = False; 

   # Call when an element starts
   def startElement(self, tag, attributes):
      self.CurrentData = tag
      if tag == "ItemGroup":
         self.inTagItemGroup = True;
      elif tag == "ClCompile" and self.inTagItemGroup == True:
         path = attributes["Include"]
         if path.find("win32") < 0 :
            path = path.replace("..\iwin", ".")
            outputFile.write("%s \\\n" % path.replace("\\", "/"))
      # print "startElement tag: ", tag

   # Call when an elements ends
   def endElement(self, tag):
      if tag == "ItemGroup":
         self.inTagItemGroup = False;
      # print "endElement tag: ", tag
      self.CurrentData = ""

if ( __name__ == "__main__"):
   
   # create an XMLReader
   parser = xml.sax.make_parser()
   # turn off namepsaces
   parser.setFeature(xml.sax.handler.feature_namespaces, 0)

   # override the default ContextHandler
   Handler = VisualParser()
   parser.setContentHandler( Handler )
   outputFile = open("../../proj.android-studio/app/jni/Android.mk", "w")
   android_header = open("android_header.txt", "r")
   outputFile.write(android_header.read())

   parser.parse("../../proj.win32/iwin_win32.vcxproj")
   outputFile.write("./Classes/Platform/android/AndroidPlatform.cpp \\\n")
   outputFile.write("hellocpp/sqlite3.c \\\n")
   outputFile.write("hellocpp/Java_org_cocos2dx_cpp_IwinIapHelper.cpp \\\n")
   outputFile.write("hellocpp/Java_org_cocos2dx_cpp_AndroidPlatform.cpp \\\n")
   outputFile.write("hellocpp/main.cpp \n")

   android_footer = open("android_footer.txt", "r")
   outputFile.write(android_footer.read())

   outputFile.close()
