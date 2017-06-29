#coding:utf-8
import os, sys, glob, codecs, re, hashlib
import common

# ------------- 常量 ------------------
DECLARE_STRUCT = "struct";
DECLARE_CMSG   = "cmsg";
DECLARE_SMSG   = "smsg";
TYPE_VECTOR    = "std::vector";
TYPE_MAP       = "std::map";

# ------------- 字段定义 ------------------
# 文件
class CFile:
    name     = "";
    includes = [];
    classes  = [];
    def __init__(self):
        self.name     = "";
        self.includes = [];
        self.classes  = [];

# 类
class CClass:
    type    = "";
    name    = "";
    id      = 0;
    members = [];
    def __init__(self):
        self.type    = "";
        self.name    = "";
        self.id      = 0;
        self.members = [];

# 类成员
class CMember:
    name   = "";
    type   = "";
    vtype  = ""; # 类型是vector时的子类型
    mkey   = ""; # 类型是map时的key类型
    mvalue = ""; # 类型是map时的vlaue类型
    def __init__(self):
        self.name   = "";
        self.type   = "";
        self.vtype  = "";
        self.mkey   = "";
        self.mvalue = "";

# ------------- 全局变量 ------------------
global gDictClasses; gDictClasses = {}; # key为类名，value为文件名
global gArrFiles;    gArrFiles    = []; # 所有的文件解析对象

# -------------------------------
def IsBaseType(type):
    return (IsCoderType(type) or (type in [TYPE_VECTOR, TYPE_MAP]));

def IsCoderType(type):
    arrTypes = ["bool", "uchar", "uint", "ulong", "SBytes", "S2UInt", "S2ULong", "S3UInt", "S3ULong", "std::string",
                "BoolList", "UCharList", "UIntList", "ULongList", "S2UIntList", "S2ULongList", "S3UIntList", "S3ULongList", "StringList",
                "UIntBoolMap", "UIntUCharMap", "UIntUIntMap", "UIntULongMap", "UIntStringMap", "UIntS2UIntMap", "UIntS2ULongMap", "UIntS3UIntMap", "UIntS3ULongMap",
                "StringBoolMap", "StringUCharMap", "StringUIntMap", "StringULongMap", "StringStringMap", "StringS2UIntMap", "StringS2ULongMap", "StringS3UIntMap", "StringS3ULongMap"];
    return (type in arrTypes);

def GetMemberType(objMember):
    strType = "";
    if(isinstance(objMember, basestring)): # 为字符串类型
        strType = str(objMember);
    else:
        strType = objMember.type;

    if(strType == TYPE_VECTOR):
        return strType + "<" + objMember.vtype + ">";
    if(strType == TYPE_MAP):
        return strType + "<" + objMember.mkey + ", " + objMember.mvalue + ">";
    else:
        return strType;

def ERROR(file, msg):
    print("In file <" + file + ".txt>, ERROR: " + msg);
    exit();

def ParseFile(strFile):
    # 获得所有的有效行
    arrLines = [];
    fread = open(strFile, "r");
    try:
        for line in fread:
            line = line.strip();
            if((line == "") or (line[0 : 1] == "/")):
                continue;
            pos = line.find("/");
            if(pos > 0):
                line = line[0 : pos].strip();
            line = re.sub(' +', ' ', line); # 删除多余空格
            line = line.replace(";", ""); # 替换所有分号
            arrLines.append(line);
    finally:
        fread.close();

    # 开始解析文件
    objFile = CFile();
    objFile.name = strFile[(strFile.rindex("/") + 1) : strFile.rindex(".")];
    i = 0;
    while(i < len(arrLines)):
        strLine = arrLines[i];
        strKey = strLine[0 : strLine.index(" ")];
        if(strKey == DECLARE_STRUCT):
            found = 0;
            j = i + 2;
            while(j < len(arrLines)):
                if(arrLines[j] == "}"): # 结构体定义结束标记
                    found = 1;
                    break;
                elif(arrLines[j] == "{"): # 已经是其他结构的开始
                    break;
                else:
                    j += 1;
            if(found > 0):
                strStruct = strLine.replace(strKey + " ", "").strip();
                objStruct = ParseStruct(objFile.name, strStruct, arrLines, i + 2, j);
                objFile.classes.append(objStruct);
                i = j + 1;
            else:
                ERROR(objFile.name, "can not find end of struct for: " + strLine);
        elif((strKey == DECLARE_CMSG) or (strKey == DECLARE_SMSG)):
            strLine = strLine.replace(strKey + " ", "").strip();
            objMsg = ParseMsg(objFile.name, strKey, strLine);
            objFile.classes.append(objMsg);
            i += 1;
        else:
            ERROR(objFile.name, "unknow declare key[" + strKey + "] for line: " + strLine);
    gArrFiles.append(objFile);


def ParseStruct(strFile, strStruct, arrLines, begin, end):
    if(gDictClasses.has_key(strStruct)):
        ERROR(strFile, "redefine struct: " + strStruct);
    else:
        gDictClasses[strStruct] = strFile;

    objStruct = CClass();
    objStruct.name = strStruct;
    for i in range(begin, end):
        objMember = ParseMember(strFile, strStruct, arrLines[i]);
        objStruct.members.append(objMember);
    return objStruct;


def ParseMsg(strFile, strType, strLine):
    #print("++++MSG(" + strType + "): " + strLine);
    posLeft  = strLine.find("(");
    posRight = strLine.find(")");
    if((posLeft < 0) or (posRight < 0)):
        ERROR(strFile, "error declare msg: " + strLine);

    objMsg = CClass();
    strMsg = strLine[0 : posLeft].strip();
    if(gDictClasses.has_key(strMsg)):
        ERROR(strFile, "redefine msg: " + strMsg);
    else:
        gDictClasses[strMsg] = strFile;
    
    objMsg.type = strType;
    objMsg.name = strMsg;
    arrMembers = SplitMembers(strFile, objMsg.name, strLine[posLeft + 1 : posRight].strip());
    for strMember in arrMembers:
        objMember = ParseMember(strFile, objMsg.name, strMember);
        objMsg.members.append(objMember);
    return objMsg;


def ParseMember(strFile, strClass, strMember):
    objMember = CMember();
    posName = strMember.rfind(" ");
    if(posName < 0):
        ERROR(strFile, "class(" + strClass + "), member: " + strMember);
    objMember.name = strMember[posName + 1 : len(strMember)].strip();

    strType  = strMember[0 : posName].replace(" ", "");
    posLeft  = strType.find("<");
    posRight = strType.find(">");
    if((posLeft > 0) and (posRight > 0)):
        objMember.type = strType[0 : posLeft];
        posDot = strType.find(",");
        if(posDot > 0): # map
            objMember.mkey   = strType[(posLeft + 1) : posDot];
            objMember.mvalue = strType[(posDot  + 1) : posRight];
        else: #vector
            objMember.vtype = strType[(posLeft + 1) : posRight];
    else:
        objMember.type = strType;
    #print("#########: (" + strMember + "), " + objMember.type + ", " + objMember.name + ", " + objMember.vtype + ", " + objMember.mkey + ", " + objMember.mvalue);
    return objMember;


def SplitMembers(strFile, strClass, strMembers):
    arrRet = [];
    if(strMembers == ""):
        return arrRet;
    if(strMembers.find(",") < 0):
        arrRet.append(strMembers);
        return arrRet;

    arrPosDot = [];
    hasLeft = 0;
    for i in range(0, len(strMembers)):
        strChar = strMembers[i : i + 1];
        if(strChar == "<"):
            hasLeft = 1;
        elif(strChar == ">"):
            hasLeft = 0;
        elif((strChar == ",") and (hasLeft == 0)):
            arrPosDot.append(i);

    posFrom = 0;
    for posDot in arrPosDot:
        arrRet.append(strMembers[posFrom : posDot].strip());
        posFrom = posDot + 1;
    arrRet.append(strMembers[posFrom : ].strip()); # 最后一个
    return arrRet;


def UpdateRelations():
    # 更新头文件
    dictTemp = {};
    for objFile in gArrFiles:
        dictTemp.clear();
        for objClass in objFile.classes:
            for objMember in objClass.members:
                arrTemp = [objMember.type, objMember.vtype, objMember.mvalue];
                for strType in arrTemp:
                    if((strType != "") and (not IsBaseType(strType))):
                        if(not gDictClasses.has_key(strType)):
                            ERROR(objFile.name, "unknow type: " + strType);
                        elif(gDictClasses[strType] != objFile.name):
                            dictTemp[gDictClasses[strType]] = 1;
        for strKey in dictTemp:
            objFile.includes.append(strKey);

    # 生成消息码
    dictTemp.clear();
    for objFile in gArrFiles:
        for objClass in objFile.classes:
            if(objClass.type != ""):
                mdf = hashlib.md5();
                mdf.update(objClass.name);
                strValue = mdf.hexdigest();
                code = 0;
                for i in range(0, len(strValue)):
                    asc = ord(strValue[i : i + 1]);
                    code += (i*i*i*asc + 8*i*i + 24*i);
                # 重复判断
                if(dictTemp.has_key(code)):
                    ERROR(objFile.name, "duplicate msg:[" + str(code) + ", " + objClass.name + "], same code msg name:" + dictTemp[code]);
                else:
                    objClass.id = code;
                    dictTemp[code] = objClass.name;
                    #print("---:" + objClass.name + ", md5:" + strValue + ", id:" + str(objClass.id));

    # id对应名称
    hFile = "../../src/common/msg/idname.h";
    hGen  = hFile + ".gen";
    h = codecs.open(hGen, "w", "utf-8");
    h.write("#ifndef _COMMON_MSG_IDNAME_H_\n");
    h.write("#define _COMMON_MSG_IDNAME_H_\n\n");
    h.write("#include \"../core/core.h\"\n\n");
    h.write("class CMsgIdName\n");
    h.write("{\n");
    h.write("public:\n");
    h.write("\tstd::string GetMsgName(uint id) { return mIdName[id]; }\n");
    h.write("\tuint GetMsgId(const std::string &name) { return mNameId[name]; }\n\n");
    h.write("private:\n");
    h.write("\tUIntStringMap mIdName;\n");
    h.write("\tStringUIntMap mNameId;\n\n");
    h.write("\tfriend CMsgIdName& CSingleton<CMsgIdName>::Instance();\n");
    h.write("\t~CMsgIdName(){ }\n");
    h.write("\tCMsgIdName()\n");
    h.write("\t{\n");
    for objFile in gArrFiles:
        for objClass in objFile.classes:
            if(objClass.id > 0):
                strIdName = "mIdName[%d] = \"%s\";" % (objClass.id, objClass.name);
                strNameId = "mNameId[\"%s\"] = %d;" % (objClass.name, objClass.id);
                h.write("\t\t%-68s%s\n" % (strIdName, strNameId));
    h.write("\t}\n");
    h.write("};\n");
    h.write("#define theMsgIdName CSingleton<CMsgIdName>::Instance()\n");
    h.write("\n#endif");
    h.close();
    common.CompareFile(hFile, hGen);


def WriteCode(objFile):
    # ---- 头文件 ----
    hFile = "../../src/common/msg/" + objFile.name + ".h";
    hGen  = hFile + ".gen";
    h = codecs.open(hGen, "w", "utf-8");
    h.write("#ifndef _COMMON_MSG_%s_H_\n" % objFile.name.upper());
    h.write("#define _COMMON_MSG_%s_H_\n\n" % objFile.name.upper());
    # 包含的头文件
    if(len(objFile.includes) > 0):
        for strInclude in objFile.includes:
            h.write("#include \"%s.h\"\n" % strInclude);
    else:
        h.write("#include \"../core/msg.h\"\n");
    # 类或结构体声明
    for objClass in objFile.classes:
        h.write("\n// " + objClass.name + "\n");
        if(objClass.type != ""):
            h.write("class " + objClass.name + " : public CMsgBase\n");
        else:
            h.write("struct " + objClass.name + "\n");
        h.write("{\n");
        if(objClass.type != ""):
            h.write("public:\n");
        if(objClass.type != ""):
            msgType = "kMsgTypeClient";
            if(objClass.type == DECLARE_SMSG):
                msgType = "kMsgTypeSystem";
            h.write("\t%s() { header.type = %s; header.id = %d; }\n" % (objClass.name, msgType, objClass.id));
            h.write("\t~%s(){ }\n" % objClass.name);
            if(len(objClass.members) > 0):
                h.write("\npublic:\n");
        for objMember in objClass.members:
            h.write("\t" + GetMemberType(objMember) + " " + objMember.name + ";\n");
        if(objClass.type != ""):
            h.write("\nprotected:\n");
            strTemp = ";";
            if(len(objClass.members) == 0):
                strTemp = "{ }";
            h.write("\tvoid EncodeBody(CCoder &coder) const" + strTemp + "\n");
            h.write("\tvoid DecodeBody(CCoder &coder)" + strTemp + "\n");
        else:
            h.write("\n");
            h.write("\tvoid Encode(CCoder &coder) const;\n");
            h.write("\tvoid Decode(CCoder &coder);\n");
        h.write("};\n");
    h.write("\n#endif");
    h.close();
    common.CompareFile(hFile, hGen);

    # ---- 实现文件 ----
    strFile = objFile.name;
    cppFile = "../../src/common/msg/_" + strFile + "_.cpp";
    cppGen  = cppFile + ".gen";
    if(os.path.isfile(cppGen)):
        os.remove(cppGen);
    cpp = codecs.open(cppGen, "w", "utf-8");
    cpp.write("#include \"" + strFile + ".h\"\n");
    for objClass in objFile.classes:
        if(len(objClass.members) == 0):
            continue;
        cpp.write("\n//" + objClass.name + "\n");
        strBody = "";
        if(objClass.type != ""):
            strBody = "Body";
        # encode
        cpp.write("void %s::Encode%s(CCoder &coder) const\n" % (objClass.name, strBody));
        cpp.write("{\n");
        for objMember in objClass.members:
            WriteEncode(objMember, cpp);
        cpp.write("}\n");
        # decode
        cpp.write("void %s::Decode%s(CCoder &coder)\n" % (objClass.name, strBody));
        cpp.write("{\n");
        for i in range(0, len(objClass.members)):
            WriteDecode(objClass.members[i], cpp, i);
        cpp.write("}\n");
    cpp.close();
    common.CompareFile(cppFile, cppGen);

def WriteEncode(objMember, cpp):
    if(IsCoderType(objMember.type)):
        cpp.write("\tcoder << " + objMember.name + ";\n");
    elif(objMember.type == TYPE_VECTOR):
        cpp.write("\t//" + objMember.name + "\n");
        cpp.write("\tcoder << (uint)" + objMember.name + ".size();\n");
        cpp.write("\tFORLIST(" + objMember.name + ", i)\n");
        cpp.write("\t{\n");
        if(IsCoderType(objMember.vtype)):
            cpp.write("\t\tcoder << " + objMember.name + "[i];\n");
        else:
            cpp.write("\t\t" + objMember.name + "[i].Encode(coder);\n");
        cpp.write("\t\tif(coder.HasError()) { break; }\n");
        cpp.write("\t}\n");
    elif(objMember.type == TYPE_MAP):
        cpp.write("\t//" + objMember.name + "\n");
        cpp.write("\tcoder << (uint)" + objMember.name + ".size();\n");
        cpp.write("\tFORMAP(" + objMember.name + ", iter)\n");
        cpp.write("\t{\n");
        cpp.write("\t\tcoder << iter->first;\n");
        if(IsCoderType(objMember.mvalue)):
            cpp.write("\t\tcoder << iter->second;\n");
        else:
            cpp.write("\t\titer->second.Encode(coder);\n");
        cpp.write("\t\tif(coder.HasError()) { break; }\n");
        cpp.write("\t}\n");
    else:
        cpp.write("\t" + objMember.name + ".Encode(coder);\n");

def WriteDecode(objMember, cpp, index):
    if(IsCoderType(objMember.type)):
        cpp.write("\tcoder >> " + objMember.name + ";\n");
    elif(objMember.type == TYPE_VECTOR):
        strLen = "len" + str(index);
        cpp.write("\t//" + objMember.name + "\n");
        cpp.write("\t" + objMember.name + ".clear();\n");
        cpp.write("\tuint " + strLen + " = 0;\n");
        cpp.write("\tcoder >> " + strLen + ";\n");
        cpp.write("\tFOR(" + strLen + ", i)\n");
        cpp.write("\t{\n");
        cpp.write("\t\t" + GetMemberType(objMember.vtype) + " value;\n");
        if(IsCoderType(objMember.vtype)):
            cpp.write("\t\tcoder >> value;\n");
        else:
            cpp.write("\t\tvalue.Decode(coder);\n");
        cpp.write("\t\tif(coder.HasError()) { break; }\n");
        cpp.write("\t\t" + objMember.name + ".push_back(value);\n");
        cpp.write("\t}\n");
    elif(objMember.type == TYPE_MAP):
        strLen = "len" + str(index);
        cpp.write("\t//" + objMember.name + "\n");
        cpp.write("\t" + objMember.name + ".clear();\n");
        cpp.write("\tuint " + strLen + " = 0;\n");
        cpp.write("\tcoder >> " + strLen + ";\n");
        cpp.write("\tFOR(" + strLen + ", i)\n");
        cpp.write("\t{\n");
        cpp.write("\t\t" + GetMemberType(objMember.mkey) + " key;" + "\n");
        cpp.write("\t\t" + GetMemberType(objMember.mvalue) + " value;" + "\n");
        cpp.write("\t\tcoder >> key;\n");
        if(IsCoderType(objMember.mvalue)):
            cpp.write("\t\tcoder >> value;\n");
        else:
            cpp.write("\t\tvalue.Decode(coder);\n");
        cpp.write("\t\tif(coder.HasError()) { break; }\n");
        cpp.write("\t\t" + objMember.name + "[key] = value;\n");
        cpp.write("\t}\n");
    else:
        cpp.write("\t" + objMember.name + ".Decode(coder);\n");

# -------------------------------------
if __name__ == "__main__":
    reload(sys)
    sys.setdefaultencoding('utf8');

    files = glob.glob("../../../common/msg/*.txt");
    files.sort();
    for file in files:
        ParseFile(file);
    UpdateRelations();

    for objFile in gArrFiles:
        WriteCode(objFile);
