package billiards.viewer;

import billiards.database.Admin;
import billiards.wrapper.ConnectionPool;
import billiards.wrapper.Wrapper;

import java.time.Duration;
import java.lang.management.ManagementFactory;
import java.util.Optional;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;

import javafx.application.Application;
import javafx.stage.Stage;
import patternfinder.PatternFinder;

public final class Main extends Application {

    // These are initialized first (like they are in a constructor)
    // abdul 28/07/2026 [create application operation ownership before Viewer so shutdown can join it ahead of shared native resources]
    private final ExecutorService executor = Executors.newFixedThreadPool(Utils.numThreads);
    private final OperationRegistry operations = new OperationRegistry();
    private ConnectionPool pool = null;

    private final String versionNumber = "10.0.14";

    // Order is constructor, init, start, stop
    // It would be a lot simpler if these methods didn't exist, and I just did
    // stuff myself. A lot less magic that way.

    public static void main(final String[] args) {
        // Parse the user-facing thread argument before JavaFX constructs this
        // Application. The executor field below is initialized during
        // construction, so this must happen before launch().
        Utils.configureThreadCountFromArgs(args);
        launch(args);
    }

    // We currently don't use this
    @Override
    public void init() {
        Wrapper.errorLogging();
        Wrapper.configureNativeThreads(Utils.numThreads);
    }

    @Override
    public void start(final Stage mainWindow) {
        System.out.println("Threads available: " + Utils.numThreads);
        // abdul 31/07/2026 [print effective JVM memory inputs so Gradle, direct-JAR, and packaged launches can be compared]
        System.out.println("JVM input arguments: "
                + ManagementFactory.getRuntimeMXBean().getInputArguments());
        System.out.println("Effective maximum Java heap: "
                + Runtime.getRuntime().maxMemory() + " bytes");
        final DBGui dbGui = new DBGui();
        // Shows and waits until the window closes
        final Optional<String> databaseName = dbGui.getDbName();

        // IMPORTANT! Create a new garbage database if it does not already exist
        Admin.newJavaDB("garbage");

        // since we just have two programs for now, this is just a boolean telling if we're
        // using viewer or not.
        final boolean viewerSelected = dbGui.getProgram();

        databaseName.ifPresent(dbName -> {

            // 2024-06-06 Austin experimenting with thread and connection pool sizes
            pool = Admin.getConnectionPool(dbName, Utils.numThreads);
            if (viewerSelected) {
	            final Viewer viewer = new Viewer(
                        mainWindow, versionNumber, executor, pool, dbName, operations);
	            viewer.start(executor);
            } else {
            	final PatternFinder pFinder = new PatternFinder(mainWindow, versionNumber, pool, dbName);
                pFinder.start();
            }
        });
    }

    @Override
    public void stop() {

        // Stop Java work before releasing the native DB pool. Many background
        // tasks borrow SQLite connections through this pool, so destroying it
        // first can turn an ordinary close into a native use-after-free.
        boolean operationsStopped = false;
        try {
            // abdul 27/07/2026 [join operation-specific workers before stopping the shared executor or destroying the native pool]
            operationsStopped = operations.shutdownAsync(Duration.ofSeconds(30))
                    .get(35, TimeUnit.SECONDS);
        } catch (final Exception exception) {
            System.err.println(
                    "Operation shutdown did not complete cleanly: "
                            + exception.getMessage());
        }
        executor.shutdownNow();
        final boolean executorStopped = Utils.safeShutdownExecutor(executor, 30, TimeUnit.SECONDS);

        if (pool != null) {
            if (operationsStopped && executorStopped) {
                pool.destroy();
                pool = null;
            } else {
                System.err.println("Skipping native pool destroy because worker threads are still active");
            }
        }
    }
}
