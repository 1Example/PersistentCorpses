class CorpseManager
{
    static ref array<CorpseData> loadedCorpses;  // Array to hold loaded corpses
    static string CORPSE_SAVE_PATH = "$profile:PersistentCorpses/corpse_data.json";  // Path to save corpse data

    // Initialize the persistence system
    static void OnInit()
    {
        // Load the configuration for corpse lifetime
        PersistentCorpsesConfig.Load();
        
        Print("[PersistentCorpses] Loading corpse persistence system...");
        
        loadedCorpses = new array<CorpseData>();

        // Load saved corpse data from disk
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CorpseManager.LoadFromDisk, 1000, false);
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CorpseManager.RestoreCorpses, 2000, false);
    }

    // Load saved corpses from disk
    static void LoadFromDisk()
    {
        Print("[PersistentCorpses] Loading corpses from disk...");

        if (FileExist(CORPSE_SAVE_PATH))
        {
            ref array<ref CorpseData> loadedData = new array<ref CorpseData>();
            JsonFileLoader<array<ref CorpseData>>.JsonLoadFile(CORPSE_SAVE_PATH, loadedData);

            loadedCorpses = loadedData;
            Print("[PersistentCorpses] Loaded " + loadedCorpses.Count() + " corpses.");
        }
    }

    // Save corpse data to disk
    static void SaveCorpse(CorpseData corpse)
    {
        Print("[PersistentCorpses] Saving corpse data...");
        
        // Save all corpses to disk
        JsonFileLoader<array<ref CorpseData>>.JsonSaveFile(CORPSE_SAVE_PATH, loadedCorpses);
    }

    // Restore loaded corpses to the world
    static void RestoreCorpses()
    {
        Print("[PersistentCorpses] Restoring corpses to the world...");

        for (int i = 0; i < loadedCorpses.Count(); i++)
        {
            CorpseData corpse = loadedCorpses[i];
            
            // Check if the corpse is expired based on the lifetime (using config value)
            if (GetGame().GetTime() - corpse.timestamp > PersistentCorpsesConfig.corpseLifetime)
            {
                // Skip expired corpses
                Print("[PersistentCorpses] Skipping expired corpse at position: " + corpse.position.ToString());
                continue;
            }

            SpawnCorpse(corpse);
        }
    }

    // Method to spawn a corpse at a specific location
    static void SpawnCorpse(CorpseData corpse)
    {
        vector spawnPosition = corpse.position;

        // Create the corpse object in the world
        Print("[PersistentCorpses] Spawning corpse at position: " + spawnPosition.ToString());

        EntityAI spawnedCorpse = GetGame().CreateObject(corpse.characterType, spawnPosition, false, true);

        // Restore items by looping through the stored item names
        for (int i = 0; i < corpse.inventory.Count(); i++)
        {
            string itemName = corpse.inventory[i];
            Print("[PersistentCorpses] Restoring item: " + itemName);

            // Restore the item to the corpse's inventory
            EntityAI itemEntity = EntityAI.Cast(spawnedCorpse.GetInventory().CreateInInventory(itemName));
            if (itemEntity) {
                // Optionally, restore item properties like durability, quantity, etc.
            }
        }
    }

    // Capture the player's inventory when they die and store it as a corpse
    static void OnPlayerDeath(PlayerBase player)
    {
        // Get player name and character type
        string playerName = player.GetIdentity().GetName();
        string characterType = player.GetType();
        
        // Retrieve the player's inventory
        array<string> itemNames = new array<string>();
        array<EntityAI> items = new array<EntityAI>();
        player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
        
        for (int i = 0; i < items.Count(); i++)
        {
            EntityAI item = items.Get(i);
            if (item && item.GetType() != "")
            {
                itemNames.Insert(item.GetType());
            }
        }

        // Create the corpse data
        vector position = player.GetPosition();
        int timestamp = GetGame().GetTime();
        CorpseData corpse = new CorpseData(position, playerName, characterType, itemNames, timestamp);

        // Save this corpse data
        loadedCorpses.Insert(corpse);
        SaveCorpse(corpse);
        
        Print("[PersistentCorpses] Saved corpse for player: " + playerName);
    }
}