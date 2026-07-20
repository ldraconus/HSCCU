function Controller() {
    var dir = installer.value("TargetDir");

    if (installer.fileExists(dir + "/MaintenanceTool")) {
        installer.execute(dir + "/MaintenanceTool");
        gui.clickButton(buttons.CancelButton);
    }
}
