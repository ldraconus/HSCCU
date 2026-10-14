#include "fifthSkills.h"

#include <QFile>

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
    obj[Name] = v.mName;
    auto& vm = Sheet::ref().vm();
    auto& bag = v.mBag;
    auto keys = bag.keys();
    for (const auto& key: std::as_const(keys)) {
        if (bag[key].isStr()) obj[key] = bag[key].asString().str();
        else obj[key] = bag[key].asNumber();
    }
    return obj;
}

void FifthSkills::fromJson(QJsonDocument &doc) {
    static bool vmInitialized = false;

    if (!doc.isObject()) return;

    auto& vm = Sheet::ref().vm();
    QJsonObject obj = doc.object();
    if (!obj.contains(Name) || !obj[Name].isString()) return;
    QString name = v.mName = obj[Name].toString();

    if (!vmInitialized) vmInitialized = initializeVM();

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

static QMap<QWidget*, fifth::exe> sCheckCB;
static QMap<QWidget*, fifth::exe> sComboCB;
static QMap<QWidget*, fifth::exe> sEditCB;

static void myV(fifth::vm* vm) {
    auto& user = vm->user();
    user.push(vm->get("guid"));
}

static void checkBoxCb(SkillTalentOrPerk* stp, bool result) {
    auto* fs = dynamic_cast<FifthSkills*>(stp);
    if (fs == nullptr) return;
    auto& vm = Sheet::ref().vm();
    auto& user = vm.user();
    user.push(result);
    if (sCheckCB.contains(fs->sender())) { user.push(fifth::exe(fs)); user.swap(); sCheckCB[fs->sender()]->eval(&vm); user.pop(); }
}

static void comboBoxCb(SkillTalentOrPerk* stp, int result) {
    auto* fs = dynamic_cast<FifthSkills*>(stp);
    if (fs == nullptr) return;
    auto& vm = Sheet::ref().vm();
    auto& user = vm.user();
    user.push(result);
    if (sComboCB.contains(fs->sender())) { user.push(fifth::exe(fs)); user.swap(); sComboCB[fs->sender()]->eval(&vm); user.pop(); }
}

static void lineEditCb(SkillTalentOrPerk* stp, QString result) {
    auto* fs = dynamic_cast<FifthSkills*>(stp);
    if (fs == nullptr) return;
    auto& vm = Sheet::ref().vm();
    auto& user = vm.user();
    user.push(result);
    if (sEditCB.contains(fs->sender())) { user.push(fifth::exe(fs)); user.swap(); sEditCB[fs->sender()]->eval(&vm); user.pop(); }
}

static void crtChkBox(fifth::vm* vm) {
    auto& user = vm->user();
    auto s = user.top();
    if (!s.isExe()) {
        user.push(fifth::exe(nullptr));
        return;
    }

    FifthSkills* ths = (FifthSkills*)(s.asCallable());
    user.push(fifth::exe (ths->createCheckBox(ths->v.mWidget, ths->v.mLayout, "")));
}

static void crtCmbBox(fifth::vm* vm) {
    auto& user = vm->user();
    auto s = user.top();
    if (!s.isExe()) {
        user.push(fifth::exe(nullptr));
        return;
    }

    FifthSkills* ths = (FifthSkills*)(s.asCallable());
    user.push(fifth::exe(ths->createComboBox(ths->v.mWidget, ths->v.mLayout, "", { })));
}

static void crtLnEdit(fifth::vm* vm) {
    auto& user = vm->user();
    QLineEdit* lineedit = nullptr;
    auto s = user.top();
    if (!s.isExe()) {
        user.push(fifth::exe(nullptr));
        return;
    }

    FifthSkills* ths = (FifthSkills*) (s.asCallable());
    user.push(fifth::exe(ths->createLineEdit(ths->v.mWidget, ths->v.mLayout, "")));
}

static void callback(fifth::vm* v) {                            // (sp) w c -u-> (sp) w
    auto& user = v->user();
    auto c = user.pop();
    auto w = user.top();
    if (!w.isExe() || !c.isExe()) return;
    auto* wdgt = (QWidget*) w.asCallable();
    if (QLineEdit* edit = dynamic_cast<QLineEdit*>(wdgt); edit) sEditCB[edit] = c.asCallable();
    else if (QCheckBox* chbx = dynamic_cast<QCheckBox*>(wdgt); chbx) sCheckCB[chbx] = c.asCallable();
    else if (QComboBox* cmbo = dynamic_cast<QComboBox*>(wdgt); cmbo) sComboCB[cmbo] = c.asCallable();
}

bool FifthSkills::initializeVM() {
    auto& vm = Sheet::ref().vm();
    auto& user = vm.user();

    vm.addBuiltin("SkillName", [&user, &vm](fifth::vm*) {        // (sp) -u-> (sp) n
        auto* skill = (FifthSkills*)(user.pop().asCallable());
         user.push(skill->v.mName);
    });

    vm.addBuiltin("CheckBox", [&user, &vm](fifth::vm*) {         // (sp) -u-> (sp) w
        auto s = user.top();
        if (!s.isExe()) user.push(fifth::exe(nullptr));
        FifthSkills* skill = (FifthSkills*) s.asCallable();
        user.push(fifth::exe(skill->createCheckBox(skill->v.mWidget, skill->v.mLayout, "")));
    });

    vm.addBuiltin("ComboBox", [this, &user, &vm](fifth::vm*) {   // (sp) -u-> (sp) w
        auto s = user.top();
        if (!s.isExe()) user.push(fifth::exe(nullptr));
        FifthSkills* skill = (FifthSkills*) s.asCallable();
        user.push(fifth::exe(skill->createComboBox(skill->v.mWidget, skill->v.mLayout, "", { })));
    });

    vm.addBuiltin("Label", [&user, &vm](fifth::vm*) {            // (sp) -u-<> (sp) w
        auto s = user.top();
        if (!s.isExe()) user.push(fifth::exe(nullptr));
        FifthSkills* skill = (FifthSkills*) s.asCallable();
        user.push(fifth::exe(skill->createLabel(skill->v.mWidget, skill->v.mLayout, "")));
    });

    vm.addBuiltin("LineEdit", [&user, &vm](fifth::vm*) {         // (sp) -u-> (sp) w
        auto s = user.top();
        if (!s.isExe()) user.push(fifth::exe(nullptr));
        FifthSkills* skill = (FifthSkills*) s.asCallable();
        user.push(fifth::exe(skill->createLineEdit(skill->v.mWidget, skill->v.mLayout, "")));
    });

    vm.addBuiltin("Get", [&user, &vm](fifth::vm*) {              // (sp) var -u-> (sp) val
        auto v = user.pop();
        auto s = user.top();
        if (!s.isExe() || !v.isStr()) return;
        QString var = s.asString().str();
        FifthSkills* skill = (FifthSkills*) s.asCallable();
        user.push(skill->v.mBag[var]);
    });

    vm.addBuiltin("Set", [&user, &vm](fifth::vm*) { // (sp) var val -u-> (sp)
        auto x = user.pop();
        auto v = user.pop();
        auto s = user.top();
        if (!s.isExe() || !v.isStr()) return;
        QString var = s.asString().str();
        FifthSkills* skill = (FifthSkills*) s.asCallable();
        skill->v.mBag[var] = x;
    });

    vm.addBuiltin("Text", [&user, &vm](fifth::vm*) { // w s -u-> w
        auto t = user.pop();
        auto w = user.top();
        if (!w.isExe() || !t.isStr()) return;
        auto* wdgt = (QWidget*) w.asCallable();
        QString txt = t.asString().str();
        if (QLineEdit* edit = dynamic_cast<QLineEdit*>(wdgt); edit) edit->setText(txt);
        else if (QCheckBox* chbx = dynamic_cast<QCheckBox*>(wdgt); chbx) chbx->setText(txt);
        else if (QComboBox* cmbo = dynamic_cast<QComboBox*>(wdgt); cmbo) cmbo->setCurrentText(txt);
        else if (QLabel* labl = dynamic_cast<QLabel*>(wdgt); labl) labl->setText(txt);
    });

    vm.addBuiltin("Prompt", [&user, &vm](fifth::vm*) { // w s -u-> w
        auto t = user.pop();
        auto w = user.top();
        if (!w.isExe() || !t.isStr()) return;
        auto* wdgt = (QWidget*) w.asCallable();
        QString txt = t.asString().str();
        if (QLineEdit* edit = dynamic_cast<QLineEdit*>(wdgt); edit) edit->setPlaceholderText(txt);
    });

    vm.addBuiltin("Value", [&user, &vm](fifth::vm*) { // w -u-> w ?
        auto w = user.top();
        if (!w.isExe()) return;
        auto* wdgt = (QWidget*) w.asCallable();
        fifth::value txt;
        if (QLineEdit* edit = dynamic_cast<QLineEdit*>(wdgt); edit) txt = edit->text();
        else if (QCheckBox* chbx = dynamic_cast<QCheckBox*>(wdgt); chbx) txt = chbx->isChecked();
        else if (QComboBox* cmbo = dynamic_cast<QComboBox*>(wdgt); cmbo) txt = cmbo->currentIndex();
        user.push(txt);
    });

    vm.addBuiltin("SetValue", [&user, &vm](fifth::vm*) { // w ? -u-> w
        auto t = user.pop();
        auto w = user.top();
        if (!w.isExe()) return;
        auto* wdgt = (QWidget*) w.asCallable();
        if (QLineEdit* edit = dynamic_cast<QLineEdit*>(wdgt); edit) edit->setText(t.asString().str());
        else if (QCheckBox* chbx = dynamic_cast<QCheckBox*>(wdgt); chbx) chbx->setChecked(t.asNumber());
        else if (QComboBox* cmbo = dynamic_cast<QComboBox*>(wdgt); cmbo) cmbo->setCurrentIndex(t.asNumber());
    });

    vm.addImmediate("Callback", [&user, &vm](fifth::vm*) { // [ c -u-> ] { c = pop(); code->push(c); }
        if (vm.compiling()) {
            static auto func = fifth::builtin(::callback);
            auto c = user.pop();
            auto parent = (fifth::compiled*) vm.code();
            parent->push(c);
            parent->call(&func);
        }
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
    v.mBag[symbol] = code;
    return true;
}
