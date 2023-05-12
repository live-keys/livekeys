#include "lvellanguageinfoserializationtest.h"
#include "live/elements/compiler/languageinfo.h"
#include <set>
#include "live/mlnodetojson.h"
#include "live/visuallog.h"

Q_TEST_RUNNER_REGISTER(LvElLanguageInfoSerializationTest);

using namespace lv;
using namespace lv::el;

LvElLanguageInfoSerializationTest::LvElLanguageInfoSerializationTest(QObject *parent) : QObject(parent)
{

}

LvElLanguageInfoSerializationTest::~LvElLanguageInfoSerializationTest(){}

void LvElLanguageInfoSerializationTest::initTestCase()
{

}

void LvElLanguageInfoSerializationTest::enumInfoToMLNodeTest()
{
    EnumInfo einfo("TestEnum");
    einfo.addKey("RED", 2);
    einfo.addKey("GREEN", 4);
    einfo.addKey("BLUE", 6);

    MLNode result = einfo.toMLNode();

    QVERIFY(!result.isNull());
    QVERIFY(result.type() == MLNode::Object);

    auto obj = result.asObject();
    QVERIFY(obj.size() == 1);
    MLNode testenum = obj.at("TestEnum");
    QVERIFY(!testenum.isNull());
    QVERIFY(testenum.type() == MLNode::Object);
    auto teobj = testenum.asObject();

    QVERIFY(teobj.size() == 3);
    QVERIFY(teobj.at("RED").asInt() == 2);
    QVERIFY(teobj.at("GREEN").asInt() == 4);
    QVERIFY(teobj.at("BLUE").asInt() == 6);

}

void LvElLanguageInfoSerializationTest::fromMLNodeToEnumInfoTest()
{
    MLNode node = {
        {"TestEnum", {
             {"RED", 2},
             {"GREEN", 4},
             {"BLUE", 6}
        }}
    };

    EnumInfo a("dummyName");
    a.fromMLNode(node);

    QVERIFY(a.name() == "TestEnum");
    QVERIFY(a.keyCount()==3);

    std::set<Utf8> keys;
    for (int i = 0; i < 3; i++)
        keys.insert(a.key(i));

    QVERIFY(keys.find("RED") != keys.end());
    QVERIFY(keys.find("GREEN") != keys.end());
    QVERIFY(keys.find("BLUE") != keys.end());

}

void LvElLanguageInfoSerializationTest::functionInfoToMLNodeTest()
{
    FunctionInfo fi("testFunction", "int");
    fi.addParameter("a", "int");
    fi.addParameter("b", "string");
    fi.addParameter("c", "Object");

    MLNode functionNode = fi.toMLNode();

    QVERIFY(!functionNode.isNull());
    QVERIFY(functionNode.size() == 1);
    QVERIFY(functionNode.type() == MLNode::Object);

    MLNode::ObjectType obj = functionNode.asObject();
    MLNode rest = obj.at("testFunction");
    QVERIFY(!rest.isNull());
    QVERIFY(rest.size() == 2);
    QVERIFY(rest.type() == MLNode::Object);

    MLNode::ObjectType restNode = rest.asObject();
    QVERIFY(!restNode.at("return_type").isNull());
    QVERIFY(restNode.at("return_type").asString() == "int");
    QVERIFY(!restNode.at("parameters").isNull());
    QVERIFY(restNode.at("parameters").type() == MLNode::Array);

    MLNode::ArrayType arr = restNode.at("parameters").asArray();
    QVERIFY(arr.size() == 3);

    MLNode par0 = arr.at(0);
    QVERIFY(!par0.isNull());
    QVERIFY(par0.begin().key() == "a");
    QVERIFY(par0.begin().value().asString() == "int");

    MLNode par1 = arr.at(1);
    QVERIFY(!par1.isNull());
    QVERIFY(par1.begin().key() == "b");
    QVERIFY(par1.begin().value().asString() == "string");

    MLNode par2 = arr.at(2);
    QVERIFY(!par2.isNull());
    QVERIFY(par2.begin().key() == "c");
    QVERIFY(par2.begin().value().asString() == "Object");
}

void LvElLanguageInfoSerializationTest::fromMLNodeToFunctionInfoTest()
{
    MLNode node;
    std::string json = "{ \"testFunction\": {\n"
                       "    \"return_type\": \"int\",\n"
                       "    \"parameters\": [\n"
                       "    {\"a\": \"int\"}, {\"b\": \"string\"}, {\"c\": \"Object\"}]\n"
                       "}}";
    ml::fromJson(json, node);

    FunctionInfo res("","");
    res.fromMLNode(node);

    QVERIFY(res.name() == "testFunction");
    QVERIFY(res.returnType() == "int");

    QVERIFY(res.parameterCount() == 3);
    QVERIFY(res.parameter(0).first == "a");
    QVERIFY(res.parameter(0).second == "int");
    QVERIFY(res.parameter(1).first == "b");
    QVERIFY(res.parameter(1).second == "string");
    QVERIFY(res.parameter(2).first == "c");
    QVERIFY(res.parameter(2).second == "Object");
}

void LvElLanguageInfoSerializationTest::propertyInfoToMLNodeTest()
{
    PropertyInfo pi("property", "type");

    MLNode node = pi.toMLNode();
    QVERIFY(!node.isNull());
    QVERIFY(node.size() == 1);

    MLNode::ObjectType obj = node.asObject();
    QVERIFY(!obj.at("property").isNull());
    QVERIFY(obj.at("property").asString() == "type");
}

void LvElLanguageInfoSerializationTest::fromMLNodeToPropertyInfoTest()
{
    MLNode node = {{"property", "type"}};

    PropertyInfo pi("","");
    pi.fromMLNode(node);

    QVERIFY(pi.name() == "property");
    QVERIFY(pi.typeName() == "type");
}

void LvElLanguageInfoSerializationTest::importInfoToMLNodeTest()
{
    ImportInfo i({Utf8("a"), Utf8("b"), Utf8("c")}, Utf8("Alias"), true);

    MLNode result = i.toMLNode();

    QVERIFY(!result.isNull());
    QVERIFY(result.type() == MLNode::Object);

    MLNode::ObjectType obj = result.asObject();
    QVERIFY(obj.size() == 3);

    QVERIFY(!obj.at("segments").isNull());
    auto segs = obj.at("segments");
    QVERIFY(segs.size() == 3);
    QVERIFY(segs.type() == MLNode::Array);
    MLNode::ArrayType segsnode = segs.asArray();

    QVERIFY(!segsnode.at(0).isNull());
    QVERIFY(segsnode.at(0).asString() == "a");

    QVERIFY(!segsnode.at(1).isNull());
    QVERIFY(segsnode.at(1).asString() == "b");

    QVERIFY(!segsnode.at(2).isNull());
    QVERIFY(segsnode.at(2).asString() == "c");

    QVERIFY(!obj.at("import_as").isNull());
    QVERIFY(obj.at("import_as").asString() == "Alias");
}

void LvElLanguageInfoSerializationTest::fromMLNodeToImportInfoNoAliasTest()
{
    std::string json = "{\"segments\": [\"a\",\"b\",\"c\"], \"is_relative\": false}";
    MLNode node;

    ml::fromJson(json, node);

    std::vector<Utf8> dummy;
    ImportInfo ii(dummy);

    ii.fromMLNode(node);

    QVERIFY(ii.isRelative() == false);
    QVERIFY(ii.segments().size() == 3);

    QVERIFY(ii.segmentAt(0) == "a");
    QVERIFY(ii.segmentAt(1) == "b");
    QVERIFY(ii.segmentAt(2) == "c");

    QVERIFY(ii.importAs() == "");


}

void LvElLanguageInfoSerializationTest::fromMLNodeToImportInfoTest()
{
    std::string json = "{\"segments\": [\"package\"], \"is_relative\": true, \"import_as\": \"Qualifier\"}";
    MLNode node;

    ml::fromJson(json, node);

    std::vector<Utf8> dummy;
    ImportInfo ii(dummy);

    ii.fromMLNode(node);

    QVERIFY(ii.isRelative() == true);
    QVERIFY(ii.segments().size() == 1);
    QVERIFY(ii.segmentAt(0) == "package");
    QVERIFY(ii.importAs() == "Qualifier");


}

void LvElLanguageInfoSerializationTest::typeInfoToMLNodeTest()
{
    TypeInfo::Ptr ti = TypeInfo::create("Type", "Element", false, false);

    PropertyInfo pi1("a", "string");
    PropertyInfo pi2("b", "Color");
    ti->addProperty(pi1);
    ti->addProperty(pi2);

    FunctionInfo fi1("func", "Element");
    fi1.addParameter("elName", "string");
    ti->addFunction(fi1);

    FunctionInfo fe1("cChanged", "");
    ti->addEvent(fe1);
    FunctionInfo fe2("dChanged", "void");
    ti->addEvent(fe2);

    FunctionInfo cons("constructor", "");
    cons.addParameter("m", "int");
    ti->setConstructor(cons);

    MLNode result = ti->toMLNode();

    QVERIFY(!result.isNull());
    QVERIFY(result.type() == MLNode::Object);
    MLNode::ObjectType object = result.asObject();

    QVERIFY(object.find("type_name") != object.end());
    QVERIFY(object.at("type_name").asString() == "Type");

    QVERIFY(object.find("inherits") != object.end());
    QVERIFY(object.at("inherits").asString() == "Element");

    QVERIFY(object.find("is_creatable") != object.end());
    QVERIFY(object.at("is_creatable").asBool() == false);

    QVERIFY(object.find("is_instance") != object.end());
    QVERIFY(object.at("is_instance").asBool() == false);

    QVERIFY(object.find("properties") != object.end());
    QVERIFY(object.at("properties").size() == 2);

    QVERIFY(object.find("functions") != object.end());
    QVERIFY(object.at("functions").size() == 1);

    QVERIFY(object.find("methods") != object.end());
    QVERIFY(object.at("methods").size() == 0);

    QVERIFY(object.find("events") != object.end());
    QVERIFY(object.at("events").size() == 2);
}

void LvElLanguageInfoSerializationTest::fromMLNodeToTypeInfoTest()
{
    std::string json = "{\"class_name\":\"\","
                       "\"constructor\":{\"constructor\":{\"parameters\":[{\"m\":\"int\"}],\"return_type\":\"\"}},"
                       "\"events\":[{\"cChanged\":{\"parameters\":[],\"return_type\":\"\"}},{\"dChanged\":{\"parameters\":[],\"return_type\":\"void\"}}],"
                       "\"functions\":[{\"func\":{\"parameters\":[{\"elName\":\"string\"}],\"return_type\":\"Element\"}}],"
                       "\"inherits\":\"Element\","
                       "\"is_creatable\":false,"
                       "\"is_instance\":false,"
                       "\"methods\":[],"
                       "\"properties\":[{\"a\":\"string\"},{\"b\":\"Color\"}],"
                       "\"type_name\":\"Type\"}";
    MLNode node;
    ml::fromJson(json, node);

    TypeInfo::Ptr ti = TypeInfo::create("","", false, false);
    ti->fromMLNode(node);

    QVERIFY(ti->typeName() == "Type");
    QVERIFY(ti->inheritsName() == "Element");
    QVERIFY(ti->isCreatable() == false);
    QVERIFY(ti->isInstance() == false);

    QVERIFY(ti->totalEvents() == 2);
    QVERIFY(ti->totalMethods() == 0);
    QVERIFY(ti->totalFunctions() == 1);
    QVERIFY(ti->totalProperties() == 2);
}

void LvElLanguageInfoSerializationTest::inheritanceInfoToMLNodeTest()
{
    InheritanceInfo ii;
    ii.addType(TypeInfo::create("a","", true, true));
    ii.addType(TypeInfo::create("b", "Container", false, true));

    MLNode result = ii.toMLNode();

    QVERIFY(!result.isNull());
    QVERIFY(result.type() == MLNode::Array);

    auto array = result.asArray();
    QVERIFY(array.size() == 2);

    QVERIFY(array.at(0).asObject().at("type_name").asString() == "a");
    QVERIFY(array.at(1).asObject().at("type_name").asString() == "b");

}

void LvElLanguageInfoSerializationTest::fromMLNodeToInheritanceInfoTest()
{
    std::string json = "[{\"class_name\":\"\",\"constructor\":{\"\":{\"parameters\":[],\"return_type\":\"\"}},\"events\":[],\"functions\":[],\"is_creatable\":true,\"is_instance\":true,\"methods\":[],\"properties\":[],\"type_name\":\"a\"},{\"class_name\":\"\",\"constructor\":{\"\":{\"parameters\":[],\"return_type\":\"\"}},\"events\":[],\"functions\":[],\"inherits\":\"Container\",\"is_creatable\":false,\"is_instance\":true,\"methods\":[],\"properties\":[],\"type_name\":\"b\"}]";
    MLNode node;

    ml::fromJson(json, node);

    InheritanceInfo ii;
    ii.fromMLNode(node);

    QVERIFY(ii.totalTypes() == 2);
}


void LvElLanguageInfoSerializationTest::documentInfoToMLNodeTest()
{
    auto di = DocumentInfo::create();
    di->addType(TypeInfo::create("a", "b", false, false));
    di->addType(TypeInfo::create("b", "c", true, false));

    di->addImport(ImportInfo({"a"}));
    di->addImport(ImportInfo({"a", "c"}));

    di->updateScanStatus(DocumentInfo::ScanStatus::Ready);

    MLNode res = di->toMLNode();

    QVERIFY(!res.isNull());
    QVERIFY(res.type() == MLNode::Object);
    auto object = res.asObject();

    QVERIFY(object.find("imports") != object.end());
    QVERIFY(object.at("imports").asArray().size() == 2);


    QVERIFY(object.find("types") != object.end());
    QVERIFY(object.at("types").asArray().size() == 2);

    QVERIFY(object.at("status").asInt() ==DocumentInfo::ScanStatus::Ready);
}

void LvElLanguageInfoSerializationTest::fromMLNodeToDocumentInfoTest()
{
    std::string json = "{\"imports\":[{\"is_relative\":false,\"segments\":[\"a\"]},{\"is_relative\":false,\"segments\":[\"a\",\"c\"]}],\"status\":1,\"types\":[{\"class_name\":\"\",\"constructor\":{\"\":{\"parameters\":[],\"return_type\":\"\"}},\"events\":[],\"functions\":[],\"inherits\":\"b\",\"is_creatable\":false,\"is_instance\":false,\"methods\":[],\"properties\":[],\"type_name\":\"a\"},{\"class_name\":\"\",\"constructor\":{\"\":{\"parameters\":[],\"return_type\":\"\"}},\"events\":[],\"functions\":[],\"inherits\":\"c\",\"is_creatable\":true,\"is_instance\":false,\"methods\":[],\"properties\":[],\"type_name\":\"b\"}]}";
    MLNode node;
    ml::fromJson(json, node);

    DocumentInfo::Ptr di = DocumentInfo::create();
    di->fromMLNode(node);

    QVERIFY(di->scanStatus() == DocumentInfo::ScanStatus::Ready);
    QVERIFY(di->totalTypes() == 2);
    QVERIFY(di->totalImports() == 2);

}

void LvElLanguageInfoSerializationTest::moduleInfoToMLNodeTest()
{
    ModuleInfo::Ptr mi = ModuleInfo::create("importimport", "path");
    mi->updateScanStatus(ModuleInfo::ScanStatus::Parsed);
    mi->addDependency("package1");
    mi->addDependency("plugin2");
    mi->addDependency("module3");

    DocumentInfo::Ptr di1 = DocumentInfo::create();
    TypeInfo::Ptr ti1di1 = TypeInfo::create("type1", "", false, false);
    di1->addType(ti1di1);
    mi->addUnresolvedDocument(di1);

    TypeInfo::Ptr ti2 = TypeInfo::create("type2", "a", false, true);
    TypeInfo::Ptr ti3 = TypeInfo::create("type3", "b", false, true);
    mi->addType(ti2);
    mi->addType(ti3);

    MLNode result = mi->toMLNode();

    QVERIFY(!result.isNull());
    QVERIFY(result.type() == MLNode::Object);

    auto object = result.asObject();

    QVERIFY(object.find("import_uri") != object.end());
    QVERIFY(object.at("import_uri").asString() == "importimport");
    QVERIFY(object.find("path") != object.end());
    QVERIFY(object.at("path").asString() == "path");

    QVERIFY(object.find("status") != object.end());
    QVERIFY(object.at("status").asInt() == ModuleInfo::ScanStatus::Parsed);

    QVERIFY(object.find("dependencies") != object.end());
    QVERIFY(object.at("dependencies").asArray().size() == 3);

    QVERIFY(object.find("unresolved") != object.end());
    QVERIFY(object.at("unresolved").asArray().size() == 1);

    QVERIFY(object.find("types") != object.end());
    QVERIFY(object.at("types").asArray().size() == 2);
}

void LvElLanguageInfoSerializationTest::fromMLNodeToModuleInfoTest()
{
    std::string json = "{\"dependencies\":[\"package1\",\"plugin2\",\"module3\"],\"import_uri\":\"importimport\",\"path\":\"path\",\"status\":2,\"types\":[{\"class_name\":\"\",\"constructor\":{\"\":{\"parameters\":[],\"return_type\":\"\"}},\"events\":[],\"functions\":[],\"inherits\":\"a\",\"is_creatable\":false,\"is_instance\":true,\"methods\":[],\"properties\":[],\"type_name\":\"type2\"},{\"class_name\":\"\",\"constructor\":{\"\":{\"parameters\":[],\"return_type\":\"\"}},\"events\":[],\"functions\":[],\"inherits\":\"b\",\"is_creatable\":false,\"is_instance\":true,\"methods\":[],\"properties\":[],\"type_name\":\"type3\"}],\"unresolved\":[{\"imports\":[],\"status\":0,\"types\":[{\"class_name\":\"\",\"constructor\":{\"\":{\"parameters\":[],\"return_type\":\"\"}},\"events\":[],\"functions\":[],\"is_creatable\":false,\"is_instance\":false,\"methods\":[],\"properties\":[],\"type_name\":\"type1\"}]}]}";
    MLNode node;
    ml::fromJson(json, node);

    ModuleInfo::Ptr mi = ModuleInfo::create("","");
    mi->fromMLNode(node);

    QVERIFY(mi->importUri() == "importimport");
    QVERIFY(mi->path() == "path");
    QVERIFY(mi->scanStatus() == ModuleInfo::ScanStatus::Parsed);

    QVERIFY(mi->totalTypes() == 2);
    QVERIFY(mi->totalDependencies() == 3);
    QVERIFY(mi->totalUnresolvedDocuments() == 1);
}
