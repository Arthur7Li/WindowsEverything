package billiards.viewer;

import javafx.concurrent.Task;
import javafx.geometry.Insets;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.ProgressBar;
import javafx.scene.layout.HBox;
import javafx.stage.Modality;
import javafx.stage.Stage;


import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;

import billiards.wrapper.Wrapper;

public final class Progress {
    private final HBox root = new HBox();
    private final Button cancelButton = new Button();
    private final ProgressBar progressBar = new ProgressBar();
    private final Scene scene = new Scene(root);
    private final Stage stage = new Stage();
    // abdul 31/07/2026 [return a Viewer progress-window slot exactly once whether the window closes normally or by user cancellation]
    private final AtomicBoolean windowSlotReturned = new AtomicBoolean(false);

    // We don't care what task return type is
    public Progress(final Task<?> task) {
        root.setSpacing(10);
        root.getChildren().addAll(progressBar, cancelButton);
        root.setPadding(new Insets(10));

        stage.setScene(scene);
        stage.setOnCloseRequest(event -> {
            // abdul 31/07/2026 [make the window close control invoke the same native cancellation path as the button]
            Wrapper.requestVaryCancellation();
            requestCancel(task);
            stage.close();
        });

        // Block from accessing the main window until this stage is closed
        stage.initModality(Modality.APPLICATION_MODAL);

        progressBar.progressProperty().bind(task.progressProperty());

        cancelButton.setText("Cancel");
        cancelButton.setOnAction(event -> {
            // abdul 31/07/2026 [latch cancellation before asking the task to preserve completed partial results]
            Wrapper.requestVaryCancellation();
            requestCancel(task);
            stage.close();
        });
    }

    private static void requestCancel(final Task<?> task) {
        if (task instanceof GracefullyCancelable) {
            ((GracefullyCancelable) task).requestGracefulCancel();
        } else {
            task.cancel();
        }
    }

    public void close() {
        stage.close();
    }

    public void show() {
        stage.show();
    }

    /**
     * Returns the Viewer progress-window quota when this stage becomes hidden.
     *
     * <p>The older implementation replaced {@code setOnCloseRequest}, which
     * removed the cancellation handler installed by the constructor. It also
     * failed to return the quota after normal programmatic closes because
     * those closes do not represent a window-manager close request.</p>
     *
     * @param count shared count of progress windows that may be displayed
     */
    public void incrementWindowCount(final AtomicInteger count) {
        // abdul 31/07/2026 [observe every hidden path without replacing the constructor's close-to-cancel handler]
        stage.setOnHidden(event -> {
            // abdul 31/07/2026 [guard against repeated lifecycle notifications returning one slot more than once]
            if (windowSlotReturned.compareAndSet(false, true)) {
                // abdul 31/07/2026 [make the window slot available to a later operation after success failure or cancellation]
                count.incrementAndGet();
            }
        });
    }
}
