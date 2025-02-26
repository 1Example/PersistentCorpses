// corpse_manager.c

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
        for (int i = 0; i < corpse.items.Count(); i++)
        {
            string itemName = corpse.items[i];
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
        // Retrieve the player's inventory
        array<string> itemNames = new array<string>();

        ref array<ItemBase> itemsInInventory = player.GetInventory().GetItems();  // Get the inventory items
        for (int i = 0; i < itemsInInventory.Count(); i++)
        {
            ItemBase item = itemsInInventory[i];
            string itemName = item.GetType();  // Get the item type/name
            itemNames.Insert(itemName);  // Add the item to the list
        }

        // Create the corpse data and store the player's position
        vector position = player.GetPosition();  // Get the player's last position
        int timestamp = GetGame().GetTime();  // Get the current time for the corpse timestamp
        CorpseData corpse = new CorpseData(position, itemNames, timestamp);

        // Save this corpse data to the list
        loadedCorpses.Insert(corpse);

        // Save the corpse data to disk
        SaveCorpse(corpse);
    }
}

// Data structure for storing corpse data (position, items)
class CorpseData
{
    vector position;  // Position of the corpse in the world
    ref array<string> items;  // Items inside the corpse (dynamic)
    string characterType;  // Type of the character for restoring (e.g., SurvivorM_Mirek)
    int timestamp;  // Timestamp of when the corpse was created (used for expiration check)

    void CorpseData(vector pos, array<string> itemList, int time)
    {
        position = pos;
        items = itemList;
        characterType = "SurvivorM_Mirek"; // Default character type, can be modified
        timestamp = time;
    }

    // Serialize the corpse data (to JSON)
    string Serialize()
    {
        ref map<string, ref array<string>> dataMap = new map<string, ref array<string>>;
        dataMap["position"] = new array<string>{position.ToString()};
        dataMap["items"] = items;
        dataMap["timestamp"] = new array<string>{timestamp.ToString()};

        JsonSerializer jsonSerializer = new JsonSerializer();
        return jsonSerializer.Serialize(dataMap);
    }

    // Deserialize the corpse data from JSON (Corrected code)
    static ref CorpseData Deserialize(string serializedData)
    {
        JsonSerializer jsonSerializer = new JsonSerializer();
        ref map<string, ref array<string>> jsonData = jsonSerializer.Deserialize(serializedData);

        vector pos = vector.Parse(jsonData["position"][0]);
        ref array<string> itemList = jsonData["items"];
        int time = jsonData["timestamp"][0].ToInt();

        return new CorpseData(pos, itemList, time);
    }
}
