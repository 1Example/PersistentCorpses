class CorpseData
{
    vector position;             // Position of the corpse in the world
    string playerName;           // Name of the player associated with the corpse
    string characterType;        // Type of character (e.g., SurvivorM_Mirek)
    ref array<string> inventory; // List of items in the corpse's inventory
    int timestamp;               // Time of death (Unix timestamp)

    void CorpseData(vector pos, string name, string type, array<string> inv, int time)
    {
        position = pos;
        playerName = name;
        characterType = type;
        inventory = inv;
        timestamp = time;
    }

    // Check if the corpse is expired based on the configured lifetime
    bool IsExpired()
    {
        int currentTime = GetGame().GetTime();
        return (currentTime - timestamp) > PersistentCorpsesConfig.corpseLifetime;
    }

    // Serialize the corpse data to a string for saving to JSON
    string Serialize()
    {
        // Create a map to hold the corpse data to serialize it properly
        ref map<string, ref array<string>> dataMap = new map<string, ref array<string>>();
        
        dataMap["position"] = new array<string>{position.ToString()};
        dataMap["playerName"] = new array<string>{playerName};
        dataMap["characterType"] = new array<string>{characterType};
        dataMap["inventory"] = inventory;
        dataMap["timestamp"] = new array<string>{timestamp.ToString()};

        JsonSerializer jsonSerializer = new JsonSerializer();
        return jsonSerializer.Serialize(dataMap);
    }

    // Deserialize the corpse data from a JSON string
    static ref CorpseData Deserialize(string serializedData)
    {
        JsonSerializer jsonSerializer = new JsonSerializer();
        ref map<string, ref array<string>> dataMap = jsonSerializer.Deserialize(serializedData);

        // Deserialize the fields from the JSON map
        vector pos = vector.Parse(dataMap["position"][0]);
        string playerName = dataMap["playerName"][0];
        string characterType = dataMap["characterType"][0];
        ref array<string> inventory = dataMap["inventory"];
        int timestamp = dataMap["timestamp"][0].ToInt();

        return new CorpseData(pos, playerName, characterType, inventory, timestamp);
    }
}
