function Component()
{
    component.loaded.connect(this, Component.prototype.componentLoaded);
}

Component.prototype.componentLoaded = function()
{
    // runs when component loads
}

Component.prototype.createOperations = function()
{
    component.createOperations(); // run default operations first

    if (systemInfo.kernalType === "linux") {
        component.addOperation("CreateDesktopEntry",
            "@HomeDir@/.local/share/applications/HSCCU.desktop",
            "[Desktop Entry]\n" +
            "Type=Application\n" +
            "Name=HSCCU\n" +
            "Comment=Hero System Character Creator (Unlicensed)\n" +
            "Exec=@TargetDir@/HSCCU\n" +
            "Icon=@TargetDir@/HeroSystemIcon.png\n" +
            "Terminal=false\n" +
            "Categories=Gaming;\n" +
            "StartupNotify=true\n" +
            "StartupWMClass=HSCCU\n" +
            "Keywords=gaming;\n";
        );
    }
}
