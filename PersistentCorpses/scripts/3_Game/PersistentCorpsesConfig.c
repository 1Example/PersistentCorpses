class PersistentCorpsesConfig
{
    static int corpseLifetime = 3600; // Default: 1 hour (in seconds)
    
    // Method to load the configuration from a file
    static void Load()
    {
        string path = "$profile:PersistentCorpses/config.json";

        if (FileExist(path))
        {
            // Create a temporary instance to load the config
            ref PersistentCorpsesConfig tempConfig = new PersistentCorpsesConfig();
            JsonFileLoader<PersistentCorpsesConfig>.JsonLoadFile(path, tempConfig);
            
            // Copy the values to the static variables
            corpseLifetime = tempConfig.corpseLifetime;
            
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
        
        // Create a temporary instance to save the config
        ref PersistentCorpsesConfig tempConfig = new PersistentCorpsesConfig();
        tempConfig.corpseLifetime = corpseLifetime;
        
        JsonFileLoader<PersistentCorpsesConfig>.JsonSaveFile(path, tempConfig);
        Print("[PersistentCorpsesConfig] Default config saved: " + path);
    }
};