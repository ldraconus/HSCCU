#include "fifthSkills.h"

#include <QFile>

QMap<QString, QString> FifthSkills::sGuidMap;
QMap<QString, QString> FifthSkills::sNameMap;

FifthSkills::FifthSkills(QUrl& filename) {
    QFile file(filename.isLocalFile() ? filename.toLocalFile() : filename.toString());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QString data(file.readAll());
    file.close();
    QJsonParseError error;
    QJsonDocument json = QJsonDocument::fromJson(data.toUtf8(), &error);
    if (json.isEmpty()) {
        qDebug() << "Parse error in " + filename.toString() + ": " + error.errorString() + " at " + QString::number(error.offset);
        return;
    }
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
    static bool vmInitialized = false;
    if (!vmInitialized) vmInitialized = initializeVM();

    if (!doc.isObject()) return;

    auto& vm = Sheet::ref().vm();
    QJsonObject obj = doc.object();
    if (!obj.contains(Name) || !obj[Name].isString()) return;
    QString name = v.mName = obj[Name].toString();
    vm.createBag(name);

    compile(obj, name, Abbreviation);
    if (!compile(obj, name, Description)) return;
    if (!compile(obj, name, Form)) return;
    if (!compile(obj, name, TheCost)) return;
    if (!compile(obj, name, Restore)) return;
    if (!compile(obj, name, Roll)) return;
    if (!compile(obj, name, Store)) return;
    compile(obj, name, IsSkill);
    compile(obj, name, IsPerk);
    compile(obj, name, IsTalent);
    compile(obj, name, RED);
    compile(obj, name, RPD);

    v.mValid = true;
}

static QMap<QWidget*, fifth::exe > sCheckCB;
static QMap<QWidget*, fifth::exe > sComboCB;
static QMap<QWidget*, fifth::exe > sEditCB;

static void myV(fifth::vm* vm) {
    auto& user = vm->user();
    auto t = user.pop();
    if (!t.isExe()) {
        user.push(fifth::exe(nullptr));
        return;
    }
    FifthSkills* ths = (FifthSkills*)(t.asCallable());
    user.push(ths->myV());
}

static void checkBoxCb(SkillTalentOrPerk* stp, bool result) {
    auto* fs = dynamic_cast<FifthSkills*>(stp);
    if (fs == nullptr) return;
    auto& vm = Sheet::ref().vm();
    auto& user = vm.user();
    user.push(result);
    if (sCheckCB.contains(fs->sender())) sCheckCB[fs->sender()]->eval(&vm);
}

static void comboBoxCb(SkillTalentOrPerk* stp, int result) {
    auto* fs = dynamic_cast<FifthSkills*>(stp);
    if (fs == nullptr) return;
    auto& vm = Sheet::ref().vm();
    auto& user = vm.user();
    user.push(result);
    if (sComboCB.contains(fs->sender())) sComboCB[fs->sender()]->eval(&vm);
}

static void lineEditCb(SkillTalentOrPerk* stp, QString result) {
    auto* fs = dynamic_cast<FifthSkills*>(stp);
    if (fs == nullptr) return;
    auto& vm = Sheet::ref().vm();
    auto& user = vm.user();
    user.push(result);
    if (sEditCB.contains(fs->sender())) sEditCB[fs->sender()]->eval(&vm);
}

static void crtChkBox(fifth::vm* vm) {
    auto& user = vm->user();
    QCheckBox* checkbox = nullptr;
    auto c = user.pop();
    auto t = user.pop();
    auto s = user.pop();
    auto l = user.pop();
    auto w = user.pop();
    if (!c.isExe() || !t.isExe() || !s.isStr() || !l.isExe() || !w.isExe()) {
        user.push(fifth::exe(nullptr));
        return;
    }

    fifth::exe   cb = c.asCallable();
    FifthSkills* ths = (FifthSkills*)(t.asCallable());
    QString      string = s.asString().str();
    QVBoxLayout* layout = (QVBoxLayout*)(l.asCallable());
    QWidget*     widget = (QWidget*)(w.asCallable());
    checkbox = ths->createCheckBox(widget, layout, string, checkBoxCb);
    sCheckCB[checkbox] = cb;
}

static void crtCmbBox(fifth::vm* vm) {
    auto& user = vm->user();
    QComboBox* combobox = nullptr;
    auto c = user.pop();
    auto t = user.pop();
    auto s = user.pop();
    auto i = user.pop();
    auto l = user.pop();
    auto w = user.pop();
    if (!i.isStr() || !c.isExe() || !t.isExe() || !s.isStr() || !l.isExe() || !w.isExe()) {
        user.push(fifth::exe(nullptr));
        return;
    }

    fifth::exe   cb = c.asCallable();
    FifthSkills* ths = (FifthSkills*)(t.asCallable());
    QString      string = s.asString().str();
    QStringList  list = i.asString().str().split(":");
    QVBoxLayout* layout = (QVBoxLayout*)(l.asCallable());
    QWidget*     widget = (QWidget*)(w.asCallable());
    QComboBox* comboBox = nullptr;
    combobox = ths->createComboBox(widget, layout, string, list, comboBoxCb);
    sComboCB[combobox] = cb;
}

static void crtLnEdit(fifth::vm* vm) {
    auto& user = vm->user();
    QLineEdit* lineedit = nullptr;
    auto c = user.pop();
    auto t = user.pop();
    auto s = user.pop();
    auto l = user.pop();
    auto w = user.pop();
    if (!c.isExe() || !t.isExe() || !s.isStr() || !l.isExe() || !w.isExe()) {
        user.push(fifth::exe(nullptr));
        return;
    }

    fifth::exe   cb = c.asCallable();
    FifthSkills* ths = (FifthSkills*)(t.asCallable());
    QString      string = s.asString().str();
    QVBoxLayout* layout = (QVBoxLayout*)(l.asCallable());
    QWidget*     widget = (QWidget*)(w.asCallable());
    lineedit = ths->createLineEdit(widget, layout, string, lineEditCb);
    sEditCB[lineedit] = cb;
}

bool FifthSkills::initializeVM() {
    auto& vm = Sheet::ref().vm();
    auto& user = vm.user();

    vm.addBuiltin("skillName", [this, &user, &vm](fifth::vm*) { user.push(v.mName); });

    vm.addImmediate("createCheckBox", [this, &user, &vm](fifth::vm*) {   // p l s -u-> w
        if (vm.compiling()) {
            static auto func = fifth::builtin(crtChkBox);
            auto parent = dynamic_cast<fifth::compiled*>(vm.code());
            parent->push(fifth::exe(this));
            auto cb = vm.getBlock();
            parent->push(cb);
            parent->call(&func);
            return;
        }

        auto s = user.pop();
        auto l = user.pop();
        auto p = user.pop();
        if (!s.isStr() || !l.isExe() || !p.isExe()) user.push(fifth::exe(nullptr));

        QString      string = s.asString().str();
        QVBoxLayout* layout = (QVBoxLayout*)(l.asCallable());
        QWidget*     widget = (QWidget*)(p.asCallable());

        QCheckBox* checkbox = nullptr;
        checkbox = createCheckBox(widget, layout, string, checkBoxCb);

        sCheckCB[checkbox] = vm.getBlock();
        user.push(fifth::exe(checkbox));
    });

    vm.addImmediate("createComboBox", [this, &user, &vm](fifth::vm*) {   // p l s c -u-> w
        if (vm.compiling()) {
            static auto func = fifth::builtin(crtCmbBox);
            auto parent = dynamic_cast<fifth::compiled*>(vm.code());
            parent->push(fifth::exe(this));
            auto cb = vm.getBlock();
            parent->push(cb);
            parent->call(&func);
            return;
        }

        auto c = user.pop();
        auto s = user.pop();
        auto l = user.pop();
        auto p = user.pop();
        if (!c.isStr() || !s.isStr() || !l.isNum() || !p.isNum()) user.push(fifth::exe(nullptr));
        QString      combined = c.asString().str();
        auto         list     = combined.split(":");
        QString      string   = s.asString().str();
        QVBoxLayout* layout   = (QVBoxLayout*)(l.asCallable());
        QWidget*     widget   = (QWidget*)(p.asCallable());

        QComboBox* combobox = nullptr;
        combobox = createComboBox(widget, layout, string, list, comboBoxCb);

        sCheckCB[combobox] = vm.getBlock();
        user.push(fifth::exe(combobox));
    });

    vm.addBuiltin("createLabel", [this, &user, &vm](fifth::vm*) { // p l s -u-<> w
        auto s = user.pop();
        auto l = user.pop();
        auto p = user.pop();
        if (!s.isStr() || !l.isExe() || !p.isExe()) user.push(fifth::exe(nullptr));
        QString      string   = s.asString().str();
        QVBoxLayout* layout   = (QVBoxLayout*)(l.asCallable());
        QWidget*     widget   = (QWidget*)(p.asCallable());

        QLabel* label = nullptr;
        label = createLabel(widget, layout, string);

        user.push(fifth::exe(label));
    });

    vm.addImmediate("createLineEdit", [this, &user, &vm](fifth::vm*) { // p l s -u-> w
        if (vm.compiling()) {
            static auto func = fifth::builtin(crtLnEdit);
            auto parent = dynamic_cast<fifth::compiled*>(vm.code());
            parent->push(fifth::exe(this));
            auto cb = vm.getBlock();
            parent->push(cb);
            parent->call(&func);
            return;
        }

        auto s = user.pop();
        auto l = user.pop();
        auto p = user.pop();
        if (!s.isStr() || !l.isExe() || !p.isExe()) user.push(fifth::exe(nullptr));
        QString      string   = s.asString().str();
        QVBoxLayout* layout   = (QVBoxLayout*)(l.asCallable());
        QWidget*     widget   = (QWidget*)(p.asCallable());

        QLineEdit* lineedit = nullptr;
        lineedit = createLineEdit(widget, layout, string, lineEditCb);

        sEditCB[lineedit] = vm.getBlock();
        user.push(fifth::exe(lineedit));
    });

    vm.addImmediate("v", [this, &user, &vm](fifth::vm*) {
        if (vm.compiling()) {
            static auto func = fifth::builtin(::myV);
            auto parent = dynamic_cast<fifth::compiled*>(vm.code());
            parent->push(fifth::exe(this));
            parent->call(&func);
            return;
        }

        user.push(v.mGuid);
    });

    return true;
}

bool FifthSkills::compile(QJsonObject &obj, const QString &name, const QString &symbol) {
    auto& vm = Sheet::ref().vm();
    if (!obj.contains(symbol) || !obj[symbol].isString()) return false;
    auto input = vm.input();
    vm.setInput(obj[symbol].toString());
    auto code = vm.getBlock();
    vm.setInput(input);
    vm.set(name, symbol, code);
    return true;
}
