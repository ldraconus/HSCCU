#include "fifthSkills.h"

#include <QFile>

QMap<QString, QString> FifthSkills::sGuidMap;

FifthSkills::FifthSkills(QUrl filename) {
    QFile file(filename.isLocalFile() ? filename.toLocalFile() : filename.toString());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QByteArray data(file.readAll());
    file.close();
    QString jsonStr(data);
    QJsonDocument json = QJsonDocument::fromJson(jsonStr.toUtf8());
    fromJson(json);
}

QJsonObject FifthSkills::toJson() {
    QJsonObject obj = SkillTalentOrPerk::toJson();
    obj[Name] = sGuidMap[v.mGuid];
    auto& vm = Sheet::ref().vm();
    auto& bag = vm.bag(mGuid);
    auto keys = bag.keys();
    for (const auto& key: std::as_const(keys)) {
        if (bag[key].isStr()) obj[key.str()] = bag[key].asString().str();
        else obj[key.str()] = bag[key].asNumber();
    }
    return obj;
}

void FifthSkills::fromJson(QJsonDocument &doc) {
    if (!doc.isObject()) return;

    auto& vm = Sheet::ref().vm();
    QJsonObject obj = doc.object();
    if (!obj.contains(Name) || !obj[Name].isString()) return;
    QString name = obj[Name].toString();
    vm.createBag(name);

    compile(obj, name, Abbreviation);
    if (!compile(obj, name, Description)) return;
    if (!compile(obj, name, Form)) return;
    if (!compile(obj, name, Cost)) return;
    if (!compile(obj, name, Restore)) return;
    if (!compile(obj, name, Roll)) return;
    if (!compile(obj, name, Store)) return;
    compile(obj, name, IsSkill);
    compile(obj, name, IsPerk);
    compile(obj, name, IsTalent);
    compile(obj, name, RED);
    compile(obj, name, RPD);

    if (Bool(IsTalent))    addTalent(name, (talentBase*)(this));
    else if (Bool(IsPerk)) addPerk(name,   (perkBase*)(this));
    else                   addSkill(name,  (skillBase*)(this));
}

static QMap<QWidget*, fifth::exe > sCheckCB;
static QMap<QWidget*, fifth::exe > sComboCB;
static QMap<QWidget*, fifth::exe > sEditCB;

bool FifthSkills::initializeVM() {
    auto& vm = Sheet::ref().vm();
    auto& user = vm.user();

    vm.addImmediate("createCheckBox", [this, &user, &vm](fifth::vm* v) {
        auto s = user.pop();
        auto l = user.pop();
        auto w = user.pop();
        if (!s.isStr() || !l.isNum() || !w.isNum()) user.push(fifth::exe(nullptr));

        QString      string = s.asString().str();
        QVBoxLayout* layout = (QVBoxLayout*)(l.asNumber());
        QWidget*     widget = (QWidget*)(w.asNumber());

        QCheckBox* checkbox = nullptr;
        checkbox = createCheckBox(widget, layout, string, [this, &user, &checkbox, &vm](SkillTalentOrPerk* stp, bool result) {
            user.push(result);
            if (sCheckCB.contains(sender())) sCheckCB[sender()]->eval(&vm);
        });

        sCheckCB[checkbox] = vm.getBlock(vm["word"], "}");
        user.push(fifth::num(checkbox));
    });

    vm.addImmediate("createComboBox", [this, &user, &vm](fifth::vm* v) {
        auto b = user.pop();
        auto s = user.pop();
        auto l = user.pop();
        auto w = user.pop();
        if (!b.isStr() || !s.isStr() || !l.isNum() || !w.isNum()) user.push(fifth::exe(nullptr));
        QString      combined = b.asString().str();
        auto         list     = combined.split(":");
        QString      string   = s.asString().str();
        QVBoxLayout* layout   = (QVBoxLayout*)(l.asNumber());
        QWidget*     widget   = (QWidget*)(w.asNumber());

        QComboBox* combobox = nullptr;
        combobox = createComboBox(widget, layout, string, list, [this, &user, &combobox, &vm](SkillTalentOrPerk* stp, int result) {
            user.push(result);
            if (sComboCB.contains(sender())) sComboCB[sender()]->eval(&vm);
        });

        sCheckCB[combobox] = vm.getBlock(vm["word"], "}");
        user.push(fifth::num(combobox));
    });

    vm.addImmediate("createLabel", [this, &user, &vm](fifth::vm* v) {
        auto s = user.pop();
        auto l = user.pop();
        auto w = user.pop();
        if (!s.isStr() || !l.isNum() || !w.isNum()) user.push(fifth::exe(nullptr));
        QString      string   = s.asString().str();
        QVBoxLayout* layout   = (QVBoxLayout*)(l.asNumber());
        QWidget*     widget   = (QWidget*)(w.asNumber());

        QLabel* label = nullptr;
        label = createLabel(widget, layout, string);

        user.push(fifth::num(label));
    });

    vm.addImmediate("createLineEdit", [this, &user, &vm](fifth::vm* v) {
        auto s = user.pop();
        auto l = user.pop();
        auto w = user.pop();
        if (!s.isStr() || !l.isNum() || !w.isNum()) user.push(fifth::exe(nullptr));
        QString      string   = s.asString().str();
        QVBoxLayout* layout   = (QVBoxLayout*)(l.asNumber());
        QWidget*     widget   = (QWidget*)(w.asNumber());

        QLineEdit* lineedit = nullptr;
        lineedit = createLineEdit(widget, layout, string, [this, &user, &lineedit, &vm](SkillTalentOrPerk* stp, QString result) {
            user.push(result);
            if (sEditCB.contains(sender())) sEditCB[sender()]->eval(&vm);
        });

        sEditCB[lineedit] = vm.getBlock(vm["word"], "}");
        user.push(fifth::num(lineedit));
    });

    return true;
}

bool FifthSkills::compile(QJsonObject &obj, const QString &name, const QString &symbol) {
    auto& vm = Sheet::ref().vm();
    if (!obj.contains(symbol) || !obj[symbol].isString()) return false;
    auto word = vm["word"];
    auto input = vm.input();
    vm.setInput(obj[symbol].toString());
    auto code = vm.getBlock(word, "}");
    vm.setInput(input);
    vm.set(name, symbol, code);
    return true;
}
