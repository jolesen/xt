#coding:utf-8
import os, sys, glob, xlrd, codecs
import common

# -----------------------------------
LINE_KEY    = 1;
LINE_CLIENT = 2;
LINE_SERVER = 3;
LINE_DATA   = 4;

class CConst:
    name  = "";
    type  = "";
    value = "";

class CMember:
    name  = "";
    type  = "";
    col   = ""; # 在xls中的列
    const = ""; # 是否包括常量定义

global gDictConstNames;  gDictConstNames  = {};
global gDictConstValues; gDictConstValues = {};

# -----------------------------------
def IsSupportType(type):
    arrTypes = ["bool", "uint", "string", "S2UInt", "S3UInt"];
    return (type in arrTypes);

def ERROR(xlsName, sheet, msg):
    print("In file <" + xlsName + ".xls#" + sheet + ">, error: " + msg);
    exit();

def GetRealType(xlsName, sheet, type):
    index = type.find("list");
    if(index == -1):
        if(type == "string"):
            return "std::string";
        elif(IsSupportType(type)):
            return type;
        else:
            ERROR(xlsName, sheet, "unknow type: " + type);
    else:
        listType = type[index + len("list|") : len(type)];
        ret = "std::vector<"+ GetRealType(xlsName, sheet, listType) + ">";
        ret = ret.replace("std::vector<uint>", "UIntList");
        ret = ret.replace("std::vector<S2UInt>", "S2UIntList");
        ret = ret.replace("std::vector<S3UInt>", "S3UIntList");
        ret = ret.replace("std::vector<std::string>", "StringList");
        return ret;
    ERROR(xlsName, sheet, "unknow type:" + type);

def ParseConst(xlsName, sheet, col, type, arrConst):
    for row in range(LINE_DATA, (sheet.nrows)):
        strCell = str(sheet.cell_value(row, col)).replace(" ", "");
        posDot = strCell.rfind(",");
        if(posDot <= 0):
            continue;
        objConst = CConst();
        objConst.name = strCell[(posDot + 1) : ];
        if(gDictConstNames.has_key(objConst.name)):
            print("Error: <" + xlsName + ".xls#" + sheet.name + "> and <" + gDictConstNames[objConst.name] + "> have SAME const: " + objConst.name);
            exit();
        gDictConstNames[objConst.name] = xlsName + ".xls#" + sheet.name;
        objConst.type  = type;
        objConst.value = str(strCell[0 : posDot]);
        arrConst.append(objConst);

def ParseConstSheet(xlsName, sheet):
    hFile = "../../src/common/data/" + sheet.name + ".h";
    hGen  = hFile + ".gen";
    if(os.path.isfile(hGen)):
        os.remove(hGen);
    h = codecs.open(hGen, "w", "utf-8");
    h.write("// This file is auto generated.\n");
    h.write("#ifndef _COMMON_DATA_" + sheet.name.upper() + "_H_\n");
    h.write("#define _COMMON_DATA_" + sheet.name.upper() + "_H_\n\n");
    h.write("#include \"../core/core.h\"\n\n");
    h.write("// " + xlsName + ".xls#" + sheet.name + "\n");
    for r in range(2, (sheet.nrows)):
        strName   = str(sheet.cell_value(r, 0)).replace(" ", "");
        strType   = str(sheet.cell_value(r, 1)).replace(" ", "");
        strValue  = str(sheet.cell_value(r, 2)).replace(" ", "");
        strRemark = str(sheet.cell_value(r, 3)).replace(" ", "");
        if(strName == ""):
            h.write("\n");
            continue;
        if((strType != "uint") and (strType != "string")):
            print("Error: [" + xlsName + ".xls#" + sheet.name + "] error type = " + strType);
            exit();
        elif(gDictConstNames.has_key(strName)):
            print("Error: [" + xlsName + ".xls#" + sheet.name + "] and [" + gDictConstNames[strName] + "] have SAME const: " + strName);
            exit();
        gDictConstNames[strName] = xlsName + ".xls#" + sheet.name;
        strFlag = "";
        if(strType == "string"):
            strFlag = "\"";
        else:
            if(strValue.find(".") >= 0):
                strValue = strValue[0 : strValue.find(".")];
        if(strValue == ""): # 如果值为空就自动生成
            if(strType != "uint"):
                print("Error: [" + xlsName + ".xls#" + sheet.name + "] <" + strName + "> isn't uint type, must given value");
                exit();
            code = 1000000;
            for i in range(0, len(strName)):
                asc = ord(strName[i : i + 1]);
                code += (pow(2, i / 2) * asc * i + i * i * i);
            if(gDictConstValues.has_key(code)):
                print("Error: code=" + str(code) + " HAD const=" + gDictConstValues[strName]);
                exit();
            gDictConstValues[code] = strName;
            strValue = str(code);
        strConst = "const " + GetRealType(xlsName, sheet, strType) + " " + strName + " = " + strFlag + strValue + strFlag + ";";
        if(strRemark != ""):
            strConst = strConst + " // " + strRemark;
        h.write(strConst + "\n");
    h.write("\n#endif");
    h.close();
    common.CompareFile(hFile, hGen);

def GenCodeAndJson(xlsName, sheet):
    # 判断是否为常量sheet
    if(str(sheet.row_values(0)[0]).replace(" ", "") == "const"):
        ParseConstSheet(xlsName, sheet);
        return;

    # 最多有两个key
    priName = "";
    priType = "";
    subName = "";
    subType = "";
    rowKey = sheet.row_values(LINE_KEY);
    strKey = str(rowKey[0]).replace(" ", "");
    strKey = strKey[(strKey.find("(") + 1) : strKey.find(")")];
    pos = strKey.find(',');
    if(pos > 0):
        priName = strKey[0 : pos];
        subName = strKey[(pos + 1) : ];
    else:
        priName = strKey;

    # 成员
    arrMember = [];
    arrConst  = [];
    members = sheet.row_values(LINE_SERVER);
    for col in range(0, len(members)):
        strTemp = str(members[col]).replace(" ", "");
        if(strTemp == ""):
            continue;

        objMember = CMember();
        objMember.col   = col;
        objMember.const = 0;
        objMember.name  = strTemp[0 : strTemp.find("(")];
        strType = strTemp[strTemp.find("(") + 1 : strTemp.find(")")];
        posConst = strType.find(",const");
        if(posConst > 0):
            objMember.type  = strType[0 : posConst];
            objMember.const = 1;
            ParseConst(xlsName, sheet, col, objMember.type, arrConst);
        else:
            objMember.type  = strType;
        arrMember.append(objMember);
    if((len(arrMember) == 0) and (len(arrConst) == 0)):
        return;

    # 类名
    clsName = sheet.name.capitalize();
    _index  = clsName.find('_');
    while(_index >= 0):
        sub_pre = clsName[0 : _index];
        sub_suf = clsName[(_index + 1) : ];
        clsName = sub_pre[0 : 1].upper() + sub_pre[1 : ] + sub_suf[0 : 1].upper() + sub_suf[1: ];
        _index  = clsName.find('_');
    cls = "C" + clsName + "Data";

    # ---- 头文件 ----
    hFile = "../../src/common/data/" + sheet.name + ".h";
    hGen  = hFile + ".gen";
    if(os.path.isfile(hGen)):
        os.remove(hGen);
    h = codecs.open(hGen, "w", "utf-8");
    h.write("// This file is auto generated.\n");
    h.write("#ifndef _COMMON_DATA_" + sheet.name.upper() + "_H_\n");
    h.write("#define _COMMON_DATA_" + sheet.name.upper() + "_H_\n\n");
    h.write("#include \"../core/core.h\"\n\n");
    h.write("// " + xlsName + ".xls#" + sheet.name + "\n");
    # 常量
    if(len(arrConst) > 0):
        for i in range(0, len(arrConst)):
            objConst = arrConst[i];
            strFlag = "";
            if(objConst.type == "string"):
                strFlag = "\"";
            h.write("const " + GetRealType(xlsName, sheet.name, objConst.type) + " " + objConst.name + " = " + strFlag + objConst.value + strFlag + ";\n");
        h.write("\n");
    # 类声明
    h.write("class " + cls + "\n");
    h.write("{\n");
    h.write("public:\n");
    h.write("\tstruct Type\n");
    h.write("\t{\n"); 
    for i in range(0, len(arrMember)):
        objMember = arrMember[i];
        h.write("\t\t" + GetRealType(xlsName, sheet.name, objMember.type) + " " + objMember.name +";\n");
        if(objMember.name == priName):
            priType = GetRealType(xlsName, sheet.name, objMember.type);
        elif(objMember.name == subName):
            subType = GetRealType(xlsName, sheet.name, objMember.type);
    h.write("\t};\n");
    if(subType == ""):
        h.write("\ttypedef std::map<" + priType + ", Type*> DataMap;\n\n");
    else:
        h.write("\ttypedef std::map<" + subType + ", Type*>   DataMap;\n");
        h.write("\ttypedef std::map<" + priType + ", DataMap> DataMapMap;\n\n");
    h.write("public:\n");
    h.write("\tbool Load(std::string path);\n");
    if(subType == ""):
        h.write("\tconst Type* Find(" + priType + " " + priName + ");\n");
        h.write("\tconst DataMap& GetData() { return mData; }\n\n");
    else:
        h.write("\tconst Type* Find(" + priType + " " + priName + ", " + subType + " " + subName + ");\n");
        h.write("\tconst DataMap& FindData(" + priType + " " + priName + ") { return mData[" + priName + "]; }\n");
        h.write("\tconst DataMapMap& GetData() { return mData; }\n\n");
    h.write("private:\n");
    h.write("\tfriend " + cls + "& CSingleton<" + cls + ">::Instance();\n");
    h.write("\t" + cls + "() { }\n");
    h.write("\t~" + cls + "(){ }\n\n");
    if(subType == ""):
        h.write("\tDataMap mData;\n");
    else:
        h.write("\tDataMapMap mData;\n");
    h.write("};\n");
    h.write("#define the" + clsName + "Data CSingleton<" + cls + ">::Instance()\n\n");
    h.write("#endif");
    h.close();
    common.CompareFile(hFile, hGen);
    
    # ---- 实现文件 ----
    cppFile = "../../src/common/data/__" + sheet.name + ".cpp";
    cppGen  = cppFile + ".gen";
    if(os.path.isfile(cppGen)):
        os.remove(cppGen);
    cpp = codecs.open(cppGen, "w", "utf-8");
    cpp.write("// This file is auto generated.\n");
    cpp.write("#include \"../core/json.h\"\n");
    cpp.write("#include \"../core/util.h\"\n");
    cpp.write("#include \"" + sheet.name + ".h\"\n\n");
    # Load方法
    cpp.write("bool " + cls + "::Load(std::string path)\n");
    cpp.write("{\n");
    cpp.write("\tCJson json;\n");
    cpp.write("\tif(!json.Read(path + \"" + sheet.name + ".json\"))\n");
    cpp.write("\t{\n");
    cpp.write("\t\treturn false;\n");
    cpp.write("\t}\n\n");
    cpp.write("\tstd::vector<CJson::CJsonLine> &lines = json.GetLines();\n");
    cpp.write("\tFORLIST(lines, i)\n");
    cpp.write("\t{\n");
    cpp.write("\t\tStringStringMap &pairs = lines[i].pairs;\n\n");
    cpp.write("\t\tType *data = new Type;\n");
    isValuesDecled = 0;
    for i in range(0, len(arrMember)):
        objMember  = arrMember[i];
        memberName = objMember.name;
        memberType = objMember.type;
        if(memberType.find("list") < 0):
            if((memberType == "uint") or (memberType == "bool")):
                cpp.write("\t\tdata->" + memberName + " = CUtil::ToInt(pairs[\"" + memberName +"\"]);\n");
            elif(memberType == "string"):
                cpp.write("\t\tdata->" + memberName + " = pairs[\"" + memberName +"\"];\n");
            elif(memberType == "S2UInt"):
                cpp.write("\t\t{\n");
                cpp.write("\t\t\tS2UInt val;\n");
                cpp.write("\t\t\tif(sscanf(pairs[\"" + memberName + "\"].c_str(), \"%u%%%u\", &val.id, &val.value) == 2)\n");
                cpp.write("\t\t\t{\n");
                cpp.write("\t\t\t\tdata->" + memberName + " = val;\n");
                cpp.write("\t\t\t}\n");
                cpp.write("\t\t}\n");
            elif(memberType == "S3UInt"):
                cpp.write("\t\t{\n");
                cpp.write("\t\t\tS3UInt val;\n");
                cpp.write("\t\t\tif(sscanf(pairs[\"" + memberName + "\"].c_str(), \"%u%%%u%%%u\", &val.type, &val.id, &val.value) == 3)\n");
                cpp.write("\t\t\t{\n");
                cpp.write("\t\t\t\tdata->" + memberName + " = val;\n");
                cpp.write("\t\t\t}\n");
                cpp.write("\t\t}\n");
        else:
            listType = memberType[memberType.find("list") + len("list|") : len(memberType)];
            cpp.write("\n");
            if(isValuesDecled == 0):
                isValuesDecled = 1;
                cpp.write("\t\tStringList values;\n");
            cpp.write("\t\tvalues = CUtil::SplitString(pairs[\"" + memberName + "\"], \",\");\n");
            cpp.write("\t\tFORLIST(values, j)\n");
            cpp.write("\t\t{\n");
            if((listType == "uint") or (listType == "bool")):
                cpp.write("\t\t\tuint val = CUtil::ToInt(values[j]);\n");
                cpp.write("\t\t\tif(val != 0)\n");
                cpp.write("\t\t\t{\n");
                cpp.write("\t\t\t\tdata->" + memberName +".push_back(val);\n");
                cpp.write("\t\t\t}\n");
            elif(listType == "string"):
                cpp.write("\t\t\tstd::string val = values[j];\n");
                cpp.write("\t\t\tif(val != \"\")\n");
                cpp.write("\t\t\t{\n");
                cpp.write("\t\t\t\tdata->" + memberName +".push_back(val);\n");
                cpp.write("\t\t\t}\n");
            elif(listType == "S2UInt"):
                cpp.write("\t\t\tS2UInt val;\n");
                cpp.write("\t\t\tif(sscanf(values[j].c_str(), \"%u%%%u\", &val.id, &val.value) == 2)\n");
                cpp.write("\t\t\t{\n");
                cpp.write("\t\t\t\tdata->" + memberName +".push_back(val);\n");
                cpp.write("\t\t\t}\n");
            elif(listType == "S3UInt"):
                cpp.write("\t\t\tS3UInt val;\n");
                cpp.write("\t\t\tif(sscanf(values[j].c_str(), \"%u%%%u%%%u\", &val.type, &val.id, &val.value) == 3)\n");
                cpp.write("\t\t\t{\n");
                cpp.write("\t\t\t\tdata->" + memberName +".push_back(val);\n");
                cpp.write("\t\t\t}\n");
            cpp.write("\t\t}\n");
    if(subType == ""):
        cpp.write("\n\t\tmData[data->" + priName + "] = data;\n");
    else:
        cpp.write("\n\t\tmData[data->" + priName + "][data->" + subName + "] = data;\n");
    cpp.write("\t}\n\n");
    cpp.write("\treturn true;\n");
    cpp.write("}\n\n");
    # Find方法
    if(subType == ""):
        cpp.write(u"const " + cls + "::Type* " + cls + "::Find(" + priType + " " + priName + ")\n");
        cpp.write(u"{\n");
        cpp.write(u"\tIF_FIND(mData, " + priName + ", iter)\n");
        cpp.write(u"\t{\n");
        cpp.write(u"\t\treturn iter->second;\n");
        cpp.write(u"\t}\n\n");
        cpp.write(u"\treturn NULL;\n");
        cpp.write(u"}");
    else:
        cpp.write(u"const " + cls + "::Type* " + cls + "::Find(" + priType + " " + priName + ", " + subType + " " + subName + ")\n");
        cpp.write(u"{\n");
        cpp.write(u"\tIF_FIND(mData, " + priName + ", iterMap)\n");
        cpp.write(u"\t{\n");
        cpp.write(u"\t\tIF_FIND(iterMap->second, " + subName + ", iter)\n");
        cpp.write(u"\t\t{\n");
        cpp.write(u"\t\t\treturn iter->second;\n");
        cpp.write(u"\t\t}\n");
        cpp.write(u"\t}\n\n");
        cpp.write(u"\treturn NULL;\n");
        cpp.write(u"}");
    cpp.close();
    common.CompareFile(cppFile, cppGen);
    
    # ---- 生成json ----
    jsonFile = "../runtime/config/data/" + sheet.name + ".json";
    jsonGen  = jsonFile + ".gen";
    if(os.path.isfile(jsonGen)):
        os.remove(jsonGen);
    j = codecs.open(jsonGen, "w", "utf-8");
    j.write("// This file is auto generated.\n");
    j.write("{\n\"Array\":\n[\n");
    for r in range(LINE_DATA, sheet.nrows):
        j.write("\t{ ");
        for c in range(0, len(arrMember)):
            objMember = arrMember[c];
            value = sheet.cell_value(r, objMember.col);
            strValue = str(value).replace(" ", "");
            if(objMember.const):
                strValue = strValue[0 : strValue.rfind(",")];
            if(type(value) == float):
                if(strValue.find(".") >= 0):
                    strValue = strValue[0 : strValue.find(".")];
            strTmp = "\"" + objMember.name + "\":" + "\"" + strValue + "\"";
            if(c < len(arrMember) - 1):
                strTmp += ", ";
            j.write(strTmp);
        j.write(" }");
        if(r < sheet.nrows - 1):
            j.write(",");
        j.write("\n");
    j.write("]\n}\n");
    j.close();
    common.CompareFile(jsonFile, jsonGen);

# -------------------------------------
if __name__ == "__main__":
    reload(sys)
    sys.setdefaultencoding('utf8');

    # 先判断是否用同名的sheet
    dictAllSheets = {};
    files = glob.glob("../../../common/data/*.xls*");
    for file in files:
        data = xlrd.open_workbook(file);
        for sheet in data.sheets():
            if(dictAllSheets.has_key(sheet.name)):
                print("Error: [" + file + "] and [" + dictAllSheets[sheet.name] + "] have SAME sheet: " + sheet.name);
                exit();
            else:
                dictAllSheets[sheet.name]= file;
 
    for file in files:
        data = xlrd.open_workbook(file);
        for sheet in data.sheets():
            xlsName = file[file.rindex("/") + 1 : file.rindex(".")];
            GenCodeAndJson(xlsName, sheet);
