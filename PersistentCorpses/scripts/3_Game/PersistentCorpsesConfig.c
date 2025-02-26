class PersistentCorpsesConfig
{
    static int corpseLifetime = 3600; // Default: 1 hour (in seconds)

    // Method to load the configuration from a file
    static void Load()
    {
        string path = "$profile:PersistentCorpses/config.json";

        if (FileExist(path))
        {
            JsonFileLoader<PersistentCorpsesConfig>.JsonLoadFile(path, PersistentCorpsesConfig);
            Print("[PersistentCorpses] Config loaded successfully.");
        }
        else
        {
            Print("[PersistentCorpses] Config not found! Creating default config...");
            MakeDirectory("$profile:PersistentCorpses"); // Ensure folder exists
            Save();  // Save the default config if it doesn't exist
        }
    }

    // Save the config to disk
    static void Save()
    {
        string path = "$profile:PersistentCorpses/config.json";
        JsonFileLoader<PersistentCorpsesConfig>.JsonSaveFile(path, PersistentCorpsesConfig);
        Print("[PersistentCorpses] Default config saved: " + path);
    }
};
