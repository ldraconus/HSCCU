#include "fifthSkills.h"

#include <QFile>

QMap<QString, QString> FifthSkills::sGuidMap;

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
    if (!doc.isObject()) return;

    auto& vm = Sheet::ref().vm();
    QJsonObject obj = doc.object();
    if (!obj.contains(Name) || !obj[Name].isString()) return;
    QString name = obj[Name].toString();
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

    if (Bool(IsTalent, [](){ return false; }))    addTalent(name, (talentBase*)(this));
    else if (Bool(IsPerk, [](){ return false; })) addPerk(name,   (perkBase*)(this));
    else                                          addSkill(name,  (skillBase*)(this));
    v.mValid = true;
}

static QMap<QWidget*, fifth::exe > sCheckCB;
static QMap<QWidget*, fifth::exe > sComboCB;
static QMap<QWidget*, fifth::exe > sEditCB;

static void crtChkBox(fifth::vm* vm) {
    auto& user = vm->user();
    QCheckBox* checkbox = nullptr;
    auto c = user.pop();
    auto t = user.pop();
    auto s = user.pop();
    auto l = user.pop();
    auto w = user.pop();
    if (!c.isExe() || !t.isNum() || !s.isStr() || !l.isNum() || !w.isNum()) user.push(fifth::exe(nullptr));

    fifth::exe   cb = c.asCallable();
    FifthSkills* ths = (FifthSkills*)(t.asNumber());
    QString      string = s.asString().str();
    QVBoxLayout* layout = (QVBoxLayout*)(l.asNumber());
    QWidget*     widget = (QWidget*)(w.asNumber());
    checkbox = ths->createCheckBox(widget, layout, string, [ths, &user, &checkbox, &vm](SkillTalentOrPerk* stp, bool result) {
        if (sCheckCB.contains(ths->sender())) {
            user.push(result);
            sCheckCB[ths->sender()]->eval(vm);
        }
    });
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
    if (!i.isStr() || !c.isExe() || !t.isNum() || !s.isStr() || !l.isNum() || !w.isNum()) user.push(fifth::exe(nullptr));

    fifth::exe   cb = c.asCallable();
    FifthSkills* ths = (FifthSkills*)(t.asNumber());
    QString      string = s.asString().str();
    QStringList  list = i.asString().str().split(":");
    QVBoxLayout* layout = (QVBoxLayout*)(l.asNumber());
    QWidget*     widget = (QWidget*)(w.asNumber());
    QComboBox* comboBox = nullptr;
    combobox = ths->createComboBox(widget, layout, string, list, [ths, &user, &combobox, &vm](SkillTalentOrPerk* stp, int result) {
        if (sComboCB.contains(ths->sender())) {
            user.push(result);
            sComboCB[ths->sender()]->eval(vm);
        }
    });
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
    if (!c.isExe() || !t.isNum() || !s.isStr() || !l.isNum() || !w.isNum()) user.push(fifth::exe(nullptr));

    fifth::exe   cb = c.asCallable();
    FifthSkills* ths = (FifthSkills*)(t.asNumber());
    QString      string = s.asString().str();
    QVBoxLayout* layout = (QVBoxLayout*)(l.asNumber());
    QWidget*     widget = (QWidget*)(w.asNumber());
    lineedit = ths->createLineEdit(widget, layout, string, [ths, &user, &lineedit, &vm](SkillTalentOrPerk* stp, QString result) {
        if (sEditCB.contains(ths->sender())) {
            user.push(result);
            sEditCB[ths->sender()]->eval(vm);
        }
    });
    sEditCB[lineedit] = cb;
}

bool FifthSkills::initializeVM() {
    auto& vm = Sheet::ref().vm();
    auto& user = vm.user();

    vm.addImmediate("createCheckBox", [this, &user, &vm](fifth::vm*) {   // p l s -u-> w
        if (vm.compiling()) {
            static auto func = fifth::builtin(crtChkBox);
            auto parent = dynamic_cast<fifth::compiled*>(vm.code());
            parent->push(fifth::num(this));
            auto cb = vm.getBlock();
            parent->push(cb);
            parent->call(&func);
            return;
        }

        auto s = user.pop();
        auto l = user.pop();
        auto p = user.pop();
        if (!s.isStr() || !l.isNum() || !p.isNum()) user.push(fifth::exe(nullptr));

        QString      string = s.asString().str();
        QVBoxLayout* layout = (QVBoxLayout*)(l.asNumber());
        QWidget*     widget = (QWidget*)(p.asNumber());

        QCheckBox* checkbox = nullptr;
        checkbox = createCheckBox(widget, layout, string, [this, &user, &checkbox, &vm](SkillTalentOrPerk* stp, bool result) {
            user.push(result);
            if (sCheckCB.contains(sender())) sCheckCB[sender()]->eval(&vm);
        });

        sCheckCB[checkbox] = vm.getBlock();
        user.push(fifth::num(checkbox));
    });

    vm.addImmediate("createComboBox", [this, &user, &vm](fifth::vm*) {   // p l s c -u-> w
        if (vm.compiling()) {
            static auto func = fifth::builtin(crtCmbBox);
            auto parent = dynamic_cast<fifth::compiled*>(vm.code());
            parent->push(fifth::num(this));
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
        QVBoxLayout* layout   = (QVBoxLayout*)(l.asNumber());
        QWidget*     widget   = (QWidget*)(p.asNumber());

        QComboBox* combobox = nullptr;
        combobox = createComboBox(widget, layout, string, list, [this, &user, &combobox, &vm](SkillTalentOrPerk* stp, int result) {
            user.push(result);
            if (sComboCB.contains(sender())) sComboCB[sender()]->eval(&vm);
        });

        sCheckCB[combobox] = vm.getBlock();
        user.push(fifth::num(combobox));
    });

    vm.addBuiltin("createLabel", [this, &user, &vm](fifth::vm*) { // p l s -u-<> w
        auto s = user.pop();
        auto l = user.pop();
        auto p = user.pop();
        if (!s.isStr() || !l.isNum() || !p.isNum()) user.push(fifth::exe(nullptr));
        QString      string   = s.asString().str();
        QVBoxLayout* layout   = (QVBoxLayout*)(l.asNumber());
        QWidget*     widget   = (QWidget*)(p.asNumber());

        QLabel* label = nullptr;
        label = createLabel(widget, layout, string);

        user.push(fifth::num(label));
    });

    vm.addImmediate("createLineEdit", [this, &user, &vm](fifth::vm*) { // p l s -u-> w
        if (vm.compiling()) {
            static auto func = fifth::builtin(crtLnEdit);
            auto parent = dynamic_cast<fifth::compiled*>(vm.code());
            parent->push(fifth::num(this));
            auto cb = vm.getBlock();
            parent->push(cb);
            parent->call(&func);
            return;
        }

        auto s = user.pop();
        auto l = user.pop();
        auto p = user.pop();
        if (!s.isStr() || !l.isNum() || !p.isNum()) user.push(fifth::exe(nullptr));
        QString      string   = s.asString().str();
        QVBoxLayout* layout   = (QVBoxLayout*)(l.asNumber());
        QWidget*     widget   = (QWidget*)(p.asNumber());

        QLineEdit* lineedit = nullptr;
        lineedit = createLineEdit(widget, layout, string, [this, &user, &lineedit, &vm](SkillTalentOrPerk* stp, QString result) {
            user.push(result);
            if (sEditCB.contains(sender())) sEditCB[sender()]->eval(&vm);
        });

        sEditCB[lineedit] = vm.getBlock();
        user.push(fifth::num(lineedit));
    });

    vm.addBuiltin("v", [this, &user](fifth::vm*) { user.push(v.mGuid); });

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
