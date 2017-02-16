import xml.sax
import xml.etree.cElementTree as ET
import math
from xml.dom import minidom
from xml.etree.ElementTree import XML

def getNameType(name_type):
   if name_type == "ImageViewObjectData":
      return "panel"
   elif name_type == "ListViewObjectData":
      return "list_view"
   elif name_type == "PanelObjectData" or name_type == "SpriteObjectData":
      return "layout"
   elif name_type == "ButtonObjectData":
      return "button"
   elif name_type == "TextFieldObjectData":
      return "text_field"
   elif name_type == "TextBMFontObjectData" or name_type == "TextObjectData":
      return "font"
   elif name_type == "ScrollViewObjectData":
      return "scroll_view"
   else:
      return "invalid_type"

def convertValue(value):
   if value == "True":
      return "1"
   elif value == "Falese":
      return "0"
   else: 
      return value

def insertCommonValue(element, name, value):
   common = ET.SubElement(element, "common")
   common.attrib["name"] = name 
   common.attrib["value"] = value

def insertCommonFromAttr(element, name, list_attr, attr_name):
   if attr_name in list_attr:
      insertCommonValue(element, name, convertValue(list_attr[attr_name]))


def boolToValue(value):
   if value == "True":
      return "1"
   else:
      return "0"

class VisualParser( xml.sax.ContentHandler ):
   def __init__(self):
      self.cur_tag = ""
      self.inTagItemGroup = False; 

   # Call when an element starts
   def startElement(self, tag, attributes):
      self.cur_tag = tag
      print "start" , tag
      if tag == "ObjectData":
         stack_tag.append(tag)
         cur_node = ET.SubElement(stack_node[-1], "Widget")
         cur_node.attrib["name"] = attributes["Name"]
         cur_node.attrib["typeRes"] = "layer_widget"
         cur_node.attrib["id"] = "%d" % (stack_idx[-1])
         stack_idx[-1] += 1
         stack_node.append(cur_node)

         common_value = ET.SubElement(cur_node, "common_value")
         stack_common.append(common_value)

         insertCommonValue(common_value, "root", "1")
         insertCommonValue(common_value, "stick_screen", "1")

      elif tag == "AbstractNodeData":

         stack_tag.append(tag)
         cur_node = ET.SubElement(stack_node[-1], "Widget")
         cur_node.attrib["name"] = attributes["Name"]
         cur_node.attrib["typeRes"] = getNameType(attributes["ctype"])
         cur_node.attrib["id"] = "%d" % (stack_idx[-1])
         stack_idx[-1] += 1
         stack_node.append(cur_node)
         common_value = ET.SubElement(cur_node, "common_value")
         stack_common.append(common_value)

         if "Scale9Enable" in attributes:
            insertCommonValue(common_value, "scale_9", boolToValue(attributes["Scale9Enable"])  )
         if "RotationSkewX" in attributes:
            cur_node = ET.SubElement(stack_node[-1], "rotate")
            cur_node.attrib["origin"] = attributes["RotationSkewX"]
         if "Visible" in attributes:
            cur_node.attrib["visible"] = boolToValue(attributes["Visible"]) 
         if "Alpha" in attributes:
            cur_node = ET.SubElement(stack_node[-1], "opacity")
            cur_node.attrib["origin"] = attributes["Alpha"]


         if attributes["ctype"] == "ButtonObjectData":
            insertCommonFromAttr(common_value, "title", attributes, "ButtonText")
            insertCommonValue(common_value, "scale_press", "-0.05" )
            insertCommonValue(common_value, "title_size", "%d_%d" % (int(attributes["FontSize"]),int(attributes["FontSize"])))
            insertCommonFromAttr(common_value, "title_font_idx", attributes, "UserData")
            insertCommonValue(common_value, "title_alige", "1")
         elif attributes["ctype"] == "TextFieldObjectData":
            cur_node = ET.SubElement(stack_node[-1], "Size")
            cur_node.attrib["origin"] = "%i_%i" % (int(attributes["FontSize"]), int(attributes["FontSize"]))
            insertCommonValue(common_value, "source", attributes["PlaceHolderText"] )
            if "MaxLengthEnable" in attributes:
               insertCommonValue(stack_common[-1], "max_length", attributes["MaxLengthText"])
            if "PasswordEnable" in attributes:
               insertCommonValue(stack_common[-1], "pass_input", boolToValue(attributes["MaxLengthText"]))
         elif attributes["ctype"] == "TextBMFontObjectData" or attributes["ctype"] == "TextObjectData":
            insertCommonValue(stack_common[-1], "source", attributes["LabelText"])
            insertCommonFromAttr(stack_common[-1], "font_index", attributes, "UserData" )
            insertCommonValue(stack_common[-1], "bitmap_font", "1")
         elif attributes["ctype"] == "ImageViewObjectData":
            insertCommonValue(stack_common[-1], "background_type", "3")
         elif attributes["ctype"] == "ScrollViewObjectData":
            direction = "" 
            if attributes["ScrollDirectionType"] == "Vertical":
               direction = "1"
            elif attributes["ScrollDirectionType"] == "Horizontal":
               direction = "2"
            else :
               direction  = "3"
            insertCommonValue(stack_common[-1], "direction", direction)
         elif attributes["ctype"] == "ListViewObjectData":
            direction = "2" 
            if "DirectionType" in attributes:
               direction = "1"
            insertCommonValue(stack_common[-1], "direction", direction)
            insertCommonValue(stack_common[-1], "clipping_enable", boolToValue(attributes["ClipAble"]))


      else:
         if tag == "Size":
            value = "%i_%i" % (math.floor(float(attributes["X"])), math.floor(float(attributes["Y"])))
            if stack_node[-1].attrib["typeRes"] == "text_field":

               insertCommonValue(stack_common[-1], "touch_zone", value)

            elif stack_node[-1].attrib["typeRes"] == "font":
               cur_node = ET.SubElement(stack_node[-1], "Size")
               cur_node.attrib["origin"] = "%i_%i" % (math.floor(float(attributes["Y"])), math.floor(float(attributes["Y"])))
            elif stack_node[-1].attrib["typeRes"] == "list_view":
               cur_node = ET.SubElement(stack_node[-1], "Size")
               cur_node.attrib["origin"] = value
               insertCommonValue(stack_common[-1], "inner_size", value)
            else:
               cur_node = ET.SubElement(stack_node[-1], "Size")
               cur_node.attrib["origin"] = value
               if stack_tag[-1] == "ObjectData":
                  cur_node = ET.SubElement(stack_node[-1], "pos")
                  cur_node.attrib["origin"] = "%i_%i" % (math.floor(float(attributes["X"]) / 2), math.floor(float(attributes["Y"]) / 2))

               
         elif tag == "Position":
            posx = 0
            posy = 0
            if "X" in attributes:
               posx = math.floor(float(attributes["X"]))
            if "Y" in attributes:
               posy = math.floor(float(attributes["Y"]))
            cur_node = ET.SubElement(stack_node[-1], "pos")
            cur_node.attrib["origin"] = "%i_%i" % (posx, posy)
         elif tag == "CColor":
            value = "%i_%i_%i_%i" % (int(attributes["R"]), int(attributes["G"]), int(attributes["B"]), int(attributes["A"]))           
            if stack_node[-1].attrib["typeRes"] == "text_field":
               insertCommonValue(stack_common[-1], "tint_place_holder", value)
            else:
               cur_node = ET.SubElement(stack_node[-1], "color")
               cur_node.attrib["origin"] = value
         elif tag == "AnchorPoint":
            scalex = 0
            scaley = 0
            if "ScaleX" in attributes:
               scalex = float(attributes["ScaleX"]) 
            if "ScaleY" in attributes:
               scaley = float(attributes["ScaleY"])

            insertCommonValue(stack_common[-1], "anchor_point", "%.1f_%.1f" % (scalex, scaley))
         elif tag == "NormalFileData":
            insertCommonValue(stack_common[-1], "source", attributes["Path"])
         elif tag == "PressedFileData":
            insertCommonValue(stack_common[-1], "press", attributes["Path"])
         elif tag == "DisabledFileData":
            insertCommonValue(stack_common[-1], "disable", attributes["Path"])
         elif tag == "Children":
            stack_idx.append(0)
         elif tag == "FileData":
            insertCommonValue(stack_common[-1], "source", attributes["Path"])
         elif tag == "InnerNodeSize":
            insertCommonValue(stack_common[-1], "inner_size", attributes["Width"] + "_" + attributes["Height"])
         elif tag == "TextColor":
            value = "%i_%i_%i_%i" % (int(attributes["R"]), int(attributes["G"]), int(attributes["B"]), int(attributes["A"]))           
            if stack_node[-1].attrib["typeRes"] == "button":
               insertCommonValue(stack_common[-1], "title_color", value)


      # print "startElement tag: ", tag

   # Call when an elements ends
   def endElement(self, tag):
      print "end" , tag
      if tag == "ObjectData" or tag == "AbstractNodeData":
         stack_tag.pop()
         stack_node.pop()
         stack_common.pop()
      elif tag == "Children":
         stack_idx.pop()
      


def prettify(elem):
    """Return a pretty-printed XML string for the Element.
    """
    rough_string = ET.tostring(elem, 'utf-8')
    reparsed = minidom.parseString(rough_string)
    return reparsed.toprettyxml(indent="    ")

if ( __name__ == "__main__"):

   action_file = open("action.txt", "r")
   action_str = action_file.read()
   action_file.close();

   stack_node = []
   stack_tag = []
   stack_common = []
   stack_idx = []

   game = ET.Element("GAME")
   stack_node.append(game)
   cur_node = ET.SubElement(game, "MenuWidget", designSize="960_540", name="new_lobby_screen")
   action_node = XML(action_str)
   cur_node.extend(action_node)

   stack_node.append(cur_node)
   stack_idx.append(0)

   # create an XMLReader
   parser = xml.sax.make_parser()
   # turn off namepsaces
   parser.setFeature(xml.sax.handler.feature_namespaces, 0)

   # override the default ContextHandler
   Handler = VisualParser()
   parser.setContentHandler( Handler )

   parser.parse("./NewLobbyScreen.csd")

   
   output_file = open("NewLobbyScreen.xml", "w")
   output_file.write(prettify(game))
   output_file.close()
