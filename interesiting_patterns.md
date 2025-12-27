## MVC 

create a controller, communicating with the ui via a signal and a slot. E.g, when a button is pressed in the ui, a first callback (slot) is called to manage that
button press. If the action needs backend, a second signal is sent. This second signal is connected with a slot in the controller. Additional information can be added in 
the slot call as parameters for the controller to use. Once the slot of the controller gets called, the action has been delegated to the controller, instead of in the ui.
From here, the controller can call a specific backend (service) for operations. Finally, if it is considered that the operation succeeded, a signal is sent (this signal
is connecting a slot from the ui). If the operation failed, again, you can add additional parameters for the signal (like errors) for the ui to work with.
This approach is very functional, in the sense that the ui delegates the work to an invisible middleman, and only receives an output with enough information to show for the user
for example. The layers are completely isolated, and communicate via signals and slots. A simple example with real code:

### 1. VIEW (MainWindow) - Only knows about UI, emits signals when user acts

```c++
class MainWindow : public QMainWindow {
  Q_OBJECT

signals:
  // Signal to notify "someone" that user wants to save
  void saveRequested(const QString& filePath, const QString& contents);

public slots:
  // Slots to receive results from "someone"
  void onSaveSucceeded();
  void onSaveFailed(std::vector<UIError> errors);

private slots:
  // Internal slot - reacts to button click
  void onSaveButtonClicked() {
    QString path = QFileDialog::getSaveFileName(this, "Save File");
    if (!path.isEmpty()) {
      // Delegate to controller via signal - UI doesn't know HOW to save
      emit saveRequested(path, editor->toPlainText());
    }
  }
};

// Slot implementations - UI only knows how to DISPLAY results
void MainWindow::onSaveSucceeded() {
  statusBar()->showMessage("File saved!");
}

void MainWindow::onSaveFailed(std::vector<UIError> errors) {
  // Show error dialogs, for example with the Error factory explained on the next section
}
```

### 2. CONTROLLER - Middleman, orchestrates services and emits results

```c++
class Controller : public QObject {
  Q_OBJECT

signals:
  // Signals to notify view of results
  void saveSucceeded();
  void saveFailed(std::vector<UIError> errors);

public slots:
  // Slot that receives requests from view
  void onSaveRequested(const std::string& path, const std::string& contents) {
    // Delegate actual work to service
    auto result = fileService.save(contents, path);

    if (result.ok) {
      emit saveSucceeded();  // Notify view of success
    } else {
      // Convert backend errors to UI-ready errors
      auto errors = result.getAllErrorsFromTheCall();
      emit saveFailed(convertToUiErrors(errors));  // Notify view of failure
    }
  }

private:
  FileService fileService;  // Backend service
};
```

### 3. SERVICE (FileService) - Pure backend, no Qt, no UI knowledge

```c++
class FileService {
public:
  SaveResult save(const std::string& contents, const std::filesystem::path& file) {
    std::ofstream out(file);
    if (!out.is_open()) {
      return SaveResult(Error{"Could not open file", ErrType::openFailed});
    }
    out << contents;
    return SaveResult();  // ok = true
  }
};
```

### 4. WIRING (main.cpp) - Connect the layers

```c++
int main() {
  MainWindow view;
  Controller controller;

  // View -> Controller (requests)
  QObject::connect(&view, &MainWindow::saveRequested,
                   &controller, &Controller::onSaveRequested);

  // Controller -> View (responses)
  QObject::connect(&controller, &Controller::saveSucceeded,
                   &view, &MainWindow::onSaveSucceeded);
  QObject::connect(&controller, &Controller::saveFailed,
                   &view, &MainWindow::onSaveFailed);

  view.show();
  return app.exec();
}
```

## Factory translator

Factory for translating UIErrors into actual dialogs - Instead of filling the main window class with converters (effectively connecting the UIError interface with
mainWindow), create an intermediary (a factory), that knows how to translate the UIError into a dialog. For this, you can create a callable (a std::function that returns
the result of the dialog - QMessageBox::StandardButton), and some converters for the icon, the error types into the main message... and you fill the callable with the actual
construction of the dialog. Whenever the user wants to show it, it can just call it. Additionally, you can add a facility to directly call the dialog

```c++
// A function that, when called, shows a dialog and returns what button was clicked
using DialogInvoker = std::function<QMessageBox::StandardButton()>;

class ErrorDialogFactory {
public:
  // Deferred Mode - Returns callable for later invocation
  
  static DialogInvoker getDialogFromUIError(const UIError& error, QWidget* parent) {
    // Capture by value - the UIError can be destroyed, the lambda keeps its data
    QString title = error.title;
    QString body = error.body;
    ErrSeverity severity = error.severity;

    // Return a lambda that builds and shows the dialog when called
    return [parent, title, body, severity]() -> QMessageBox::StandardButton {
      QMessageBox msgBox(parent);
      msgBox.setWindowTitle(title);
      msgBox.setText(body);
      msgBox.setIcon(iconFromSeverity(severity));
      msgBox.setStandardButtons(QMessageBox::Ok);
      msgBox.exec();  // Blocks until user clicks
      return static_cast<QMessageBox::StandardButton>(msgBox.result());
    };
  }

  // Immediate Mode - Shows dialog right away (convenience)
  
  static QMessageBox::StandardButton showError(const UIError& error, QWidget* parent) {
    auto invoker = getDialogFromUIError(error, parent);
    return invoker();  // Create and immediately call
  }

  static void showErrors(const std::vector<UIError>& errors, QWidget* parent) {
    for (const auto& error : errors) {
      showError(error, parent);
    }
  }

private:
  static QMessageBox::Icon iconFromSeverity(ErrSeverity severity) {
    switch (severity) {
      case ErrSeverity::Warning: return QMessageBox::Warning;
      case ErrSeverity::Error:   return QMessageBox::Critical;
      case ErrSeverity::Fatal:   return QMessageBox::Critical;
      default:                   return QMessageBox::Information;
    }
  }
};
```

### 3. Two ways to show dialogs

```c++
// Get a callable now, invoke it later (like a Future)
auto dialogInvoker = ErrorDialogFactory::getDialogFromUIError(error, this);

// ... do other work, validation, logging ...

// Show when ready - blocks until user clicks
QMessageBox::StandardButton clicked = dialogInvoker();

// React to user choice
if (clicked == QMessageBox::Close) {
  QApplication::quit();
}


// OR 


// Show right away - simpler when you don't need deferred execution
ErrorDialogFactory::showErrors(errors, this);
```

Comparison:

```c++
  // WITHOUT FACTORY - MainWindow knows too much                                
  void MainWindow::onSaveFailed(std::vector<UIError> errors) {               
    for (auto& err : errors) {                                               
      QMessageBox msgBox(this);                                              
      msgBox.setWindowTitle(err.title);        // MainWindow knows UIError   
      msgBox.setText(err.body);                // MainWindow builds dialogs  
      if (err.severity == ErrSeverity::Error)  // MainWindow maps severity   
        msgBox.setIcon(QMessageBox::Critical);                               
      msgBox.exec();                                                         
    }                                                                        
  }                                                                          


  // WITH FACTORY - MainWindow stays clean                                      
  void MainWindow::onSaveFailed(std::vector<UIError> errors) {               
    ErrorDialogFactory::showErrors(errors, this);  // One line, delegated    
  }
```                                                                        