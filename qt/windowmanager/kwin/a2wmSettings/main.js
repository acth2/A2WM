// main.js
var reservedHeight = 100;

function adjustMaximizedWindow(window, fullScreen) {
    if (window.maximizedVertically && window.maximizedHorizontally) {
        var screenArea = workspace.clientArea(KWin.MaximizeArea, window.screen, window.desktop);

        window.geometry = {
            x: screenArea.x,
            y: screenArea.y,
            width: screenArea.width,
            height: screenArea.height - reservedHeight
        };
    }
}

workspace.clientFullScreenSet.connect((window, fullScreen) => {
    adjustMaximizedWindow(window, fullScreen);
});

workspace.clientMaximizeSet.connect((window, hMax, vMax) => {
    if (hMax && vMax) {
        adjustMaximizedWindow(window);
    }
});
