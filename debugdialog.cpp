#include "debugdialog.h"
#include "ui_debugdialog.h"

DebugDialog::DebugDialog(fifth::vm* v, fifth::exe w, QMap<QString, fifth::value>& vs, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::DebugDialog)
    , vars(vs)
    , vm(v)
    , word(w) {
    ui->setupUi(this);
    updateStack();
    updateVars();
    if (auto* c = dynamic_cast<fifth::compiled*>(word); c == nullptr) {
        ui->inPushButton->setDisabled(true);
        ui->outPushButton->setDisabled(true);
        ui->overPushButton->setDisabled(true);
        return;
    }
    connect(ui->inPushButton,   &QPushButton::clicked, this, &DebugDialog::in);
    connect(ui->outPushButton,  &QPushButton::clicked, this, & DebugDialog::out);
    connect(ui->overPushButton, &QPushButton::clicked, this, &DebugDialog::over);
    updateCode();
}

DebugDialog::~DebugDialog() {
    delete ui;
}

void DebugDialog::in() {
    if (auto* c = dynamic_cast<fifth::compiled*>(word); c == nullptr) return;
    else {
        auto instr = c->at(pc);
        if (instr.type() != fifth::instruction::EXE) return;
        auto payload = instr.payload();
        auto blk = std::get<fifth::instruction::EXE>(payload);
        auto cmp = dynamic_cast<fifth::compiled*>(blk);
        if (cmp == nullptr) return;
        stack.push({ ++pc, word });
        word = cmp;
        pc = 0;
        updateCode();
    }
}

void DebugDialog::out() {
    while (pc != -1) pc = word->step(vm, pc);
    updateStack();
    if (stack.isEmpty()) {
        ui->inPushButton->setDisabled(true);
        ui->outPushButton->setDisabled(true);
        ui->overPushButton->setDisabled(true);
        return;
    }
    else {
        auto top = stack.pop();
        pc = top.pc;
        word = top.word;
        updateCode();
    }
}

void DebugDialog::over() {
    pc = word->step(vm, pc);
    updateCode();
    updateStack();
    updateVars();
}

void DebugDialog::updateCode() {
    auto code = word->disassemble(0, vm);
    auto instrs = code.split("\n");
    ui->codeListWidget->clear();
    ui->codeListWidget->addItems(instrs);
    ui->codeListWidget->setCurrentRow(pc);
    if (pc == -1) {
        ui->inPushButton->setDisabled(true);
        ui->outPushButton->setEnabled(true);
        ui->overPushButton->setDisabled(true);
        return;
    }
    auto* c = dynamic_cast<fifth::compiled*>(word);
    auto instr = c->at(pc);
    ui->overPushButton->setEnabled(true);
    if (instr.type() == fifth::instruction::EXE) {
        auto payload = instr.payload();
        auto blk = std::get<fifth::instruction::EXE>(payload);
        auto cmp = dynamic_cast<fifth::compiled*>(blk);
        ui->inPushButton->setDisabled(cmp == nullptr);
        ui->outPushButton->setEnabled(true);
    }
}

void DebugDialog::updateStack() {
    auto view = vm->user().list().str();
    auto items = view.split("\n");
    ui->userLstWidget->clear();
    ui->userLstWidget->addItems(items);
}

void DebugDialog::updateVars() {
    ui->variableTableWidget->clear();
    auto keys = vars.keys();
    ui->variableTableWidget->setRowCount(keys.count());
    ui->variableTableWidget->setColumnCount(2);
    int row = 0;
    for (const auto& key: std::as_const(keys)) {
        QTableWidgetItem* item = nullptr;
        ui->variableTableWidget->setItem(row, 0, item = new QTableWidgetItem(key));
        item->setTextAlignment(Qt::AlignRight | Qt::AlignTop);
        ui->variableTableWidget->setItem(row, 1, item = new QTableWidgetItem(vars[key].asString().str()));
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
    }
}
