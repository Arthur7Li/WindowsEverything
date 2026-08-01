package billiards.viewer;

import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.Alert;
import javafx.scene.control.Alert.AlertType;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Priority;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.scene.text.Font;
import javafx.stage.FileChooser;
import javafx.stage.FileChooser.ExtensionFilter;
import javafx.stage.Stage;

import java.io.IOException;
import java.nio.file.Path;
import java.util.List;

public class SaveV3Window {

    // This class is created for the functionality of saving Vary 3 matching pairs when intended.

    private final Button browseBtn = new Button();
    private final Button saveMBtn = new Button();
    private final Button saveLBtn = new Button();
    private final Button clearBtn = new Button();
    private final TextField countField = new TextField("1");
    private final TextField saveToField = new TextField();
    private final HBox root = new HBox();
    private final VBox labelVBox = new VBox();
    private final HBox saveHBox = new HBox();
    private final VBox btnVBox = new VBox(10);
    private final VBox fieldVBox = new VBox();
    private final HBox upperBtnHBox = new HBox();
    public final Stage stage = new Stage();
    private final Scene scene = new Scene(root);
    private final Label saveToLabel = new Label();
    private final Label countLabel = new Label();
    private FileChooser fileChooser = new FileChooser();

    //----------------------------------------------//
    // Variables required to store default location (if specified) and count for the no of pairs to be stored.
    public int count = Integer.parseInt(countField.getText());
    public static String defaultLocation = ""; // default location is the last used
    public String location = defaultLocation;
    //----------------------------------------------//

    public SaveV3Window(final String windowTitle) {

        // Creating objects for graphical interface
        stage.setScene(scene);
        stage.setTitle(windowTitle);
        stage.setOnCloseRequest(event -> {
            stage.close();
        });

        countField.setMaxWidth(50);
        countField.setEditable(true);
        countField.setFont(Font.font("Monaco", 16));


        saveToField.setPrefColumnCount(50);
        saveToField.setEditable(true);
        saveToField.setFont(Font.font("Monaco", 16));
        saveToField.setText(defaultLocation);
        // abdul 27/07/2026 [use a save chooser that supports new text destinations while retaining editable typed paths]
        fileChooser.getExtensionFilters().add(new ExtensionFilter("Text files", "*.txt"));

        saveToLabel.setText("Save to: ");
        saveToLabel.setPadding(new Insets(5,5,10,10));

        countLabel.setText("Count: ");
        countLabel.setPadding(new Insets(5,5,5,10));

        VBox.setVgrow(saveToField, Priority.ALWAYS);

        labelVBox.getChildren().addAll(saveToLabel, countLabel);
        labelVBox.setSpacing(10);
        saveHBox.getChildren().addAll(saveMBtn, saveLBtn);
        saveHBox.setSpacing(5);
        saveHBox.setPadding(new Insets(5,5,5,5));
        upperBtnHBox.getChildren().addAll(browseBtn, clearBtn);
        upperBtnHBox.setSpacing(5);
        upperBtnHBox.setPadding(new Insets(5,5,5,5));
        btnVBox.getChildren().addAll(upperBtnHBox, saveHBox);
        btnVBox.setSpacing(10);
        btnVBox.setPadding(new Insets(0, 10, 10, 0));
        btnVBox.setAlignment(Pos.CENTER);
        fieldVBox.getChildren().addAll(saveToField, countField);
        fieldVBox.setSpacing(10);
        fieldVBox.setPadding(new Insets(0, 10, 10, 0));

        root.getChildren().addAll(labelVBox, fieldVBox, btnVBox);
        root.setSpacing(10);
        root.setPadding(new Insets(10));

        // Browse button functionality
        browseBtn.setText("Browse");
        Utils.colorButton(browseBtn, Color.SKYBLUE, Color.GOLD);
        browseBtn.setOnAction(event -> {browse();});

        // Clear button
        clearBtn.setText("Clear file");
        Utils.colorButton(clearBtn, Color.WHITE, Color.GOLD);
        clearBtn.setOnAction(event -> {clear();});

        // Save Matching button functionality
        saveMBtn.setText("Save Matching");
        Utils.colorButton(saveMBtn, Color.SKYBLUE, Color.GOLD);
        saveMBtn.setOnAction(event -> {saveM();});

        // Save Latest button functionality
        saveLBtn.setText("Save Latest Vary");
        Utils.colorButton(saveLBtn, Color.SKYBLUE, Color.GOLD);
        saveLBtn.setOnAction(event -> {saveL();});

    }

    public void show() {
        stage.show();
    }

    public void close() {
        stage.close();
    }

    private void browse() {
        // browse function in the event of browse button pressed.
        // raises Error alert if file selected is not a text file.
        // abdul 27/07/2026 [use the displayed save destination as the same path later consumed by every file command]
        final java.io.File file = fileChooser.showSaveDialog(stage);

        if (file != null) {
            String path = file.getAbsolutePath();
            if (path.endsWith(".txt")) {
                location = path;
                saveToField.setText(path);
            }
            else {
                final Alert select = new Alert(AlertType.ERROR);
                select.setContentText("Please select a valid text file.");
                select.show();
            }

        }
    }

    // abdul 27/07/2026 [route both result sources through one validated immutable export request]
    private void saveM() {
        saveCodes(List.copyOf(BoyanMenu.savePairs));
    }

    private void saveL() {
        saveCodes(List.copyOf(BoyanMenu.varySeq));
    }

    private void saveCodes(final List<String> codes) {
        // abdul 27/07/2026 [snapshot and validate path/count before atomically appending the requested result prefix]
        try {
            final Path destination = SaveV3FileService.parseDestination(saveToField.getText());
            final int requestedCount = SaveV3FileService.parseCount(countField.getText(), codes.size());
            SaveV3FileService.appendPrefixAtomically(destination, codes, requestedCount);
            rememberSuccessfulDestination(destination, requestedCount);
            stage.close();
        } catch (final IllegalArgumentException | IOException exception) {
            showFileError("Code sequences not saved.", exception);
        }
    }

    private void clear() {
        // abdul 27/07/2026 [resolve clear from the visible field and atomically replace only that selected destination]
        try {
            final Path destination = SaveV3FileService.parseDestination(saveToField.getText());
            SaveV3FileService.clearAtomically(destination);
            rememberSuccessfulDestination(destination, count);
            stage.close();
        } catch (final IllegalArgumentException | IOException exception) {
            showFileError("File not cleared.", exception);
        }
    }

    // abdul 27/07/2026 [remember path and count only after the staged replacement succeeds]
    private void rememberSuccessfulDestination(final Path destination, final int requestedCount) {
        location = destination.toString();
        defaultLocation = location;
        count = requestedCount;
        saveToField.setText(location);
    }

    // abdul 27/07/2026 [keep validation and I/O failures visible without closing the export window]
    private void showFileError(final String summary, final Exception exception) {
        final Alert select = new Alert(AlertType.ERROR);
        String message = summary;
        final String detail = exception.getMessage();
        if (detail != null && !detail.isEmpty()) {
            message += "\n" + detail;
        }
        select.setContentText(message);
        select.show();
    }
}
