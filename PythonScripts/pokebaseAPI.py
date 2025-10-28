from __future__ import annotations
import pokebase as pb
from pokebase import cache
import functools
from multiprocessing.pool import ThreadPool as Pool
import cProfile
from enum import Enum
from pathlib import Path

from unreal import AssetRegistryHelpers, AssetToolsHelpers, Object, DataAssetFactory, PrimaryDataAsset, Texture2D, Texture2DFactoryNew, PokemonRegionalFormType, EvolutionTrigger, EditorAssetLibrary, PokemonType, PokemonStatType, PokemonMoveAilment, PokemonMoveCategory, PokemonMoveLearnMethod, PokemonMoveDamageClass, PokemonMoveTarget, ItemCategory, ItemPocket, ItemAttribute, ItemFlingEffect, GrowthRateType, Array, load_object, get_default_object, uproperty, load_asset, StructBase, ustruct

cache_path = r"C:\Users\Carlos\Documents\PythonScripts"
pokemon_folder_path = "/Game/Database/Pokemons/"
ability_folder_path = "/Game/Database/Abilities/"
move_folder_path = "/Game/Database/Moves/"
item_folder_path = "/Game/Database/Items/"
evolution_chain_folder_path = "/Game/Database/EvolutionChains/"

pokemon_data_bp_asset = "/Game/Core/Blueprints/DataAssets/BP_PokemonData"
pokemon_data_bp_class = EditorAssetLibrary.load_blueprint_class(pokemon_data_bp_asset)

ability_data_bp_asset = "/Game/Core/Blueprints/DataAssets/BP_AbilityData"
ability_data_bp_class = EditorAssetLibrary.load_blueprint_class(ability_data_bp_asset)

move_data_bp_asset = "/Game/Core/Blueprints/DataAssets/BP_MoveData"
move_data_bp_class = EditorAssetLibrary.load_blueprint_class(move_data_bp_asset)

item_data_bp_asset = "/Game/Core/Blueprints/DataAssets/BP_ItemData"
item_data_bp_class = EditorAssetLibrary.load_blueprint_class(item_data_bp_asset)

evolution_chain_bp_asset = "/Game/Core/Blueprints/DataAssets/BP_EvolutionChain"
evolution_chain_bp_class = EditorAssetLibrary.load_blueprint_class(evolution_chain_bp_asset)

chain_link_bp_asset = "/Game/Core/Blueprints/BP_ChainLink"
chain_link_bp_class = EditorAssetLibrary.load_blueprint_class(chain_link_bp_asset)

def map_regional_forms(string: str) -> PokemonRegionalFormType:
    if "alola" in string:
        return PokemonRegionalFormType.ALOLAN
    elif "galar" in string:
        return PokemonRegionalFormType.GALARIAN
    elif "hisui" in string:
        return PokemonRegionalFormType.HISUIAN
    elif "paldea" in string:
        return PokemonRegionalFormType.PALDEAN
    else:
        return PokemonRegionalFormType.NONE

def map_string_to_move_learn_method(string) -> PokemonMoveLearnMethod:
    if string == "level-up":
        return PokemonMoveLearnMethod.LEVEL_UP
    elif string == "machine":
        return PokemonMoveLearnMethod.MACHINE
    elif string == "tutor":
        return PokemonMoveLearnMethod.TUTOR
    elif string == "egg":
        return PokemonMoveLearnMethod.EGG
    else:
        return PokemonMoveLearnMethod.NONE


def map_string_stat_type_method(string) -> PokemonStatType:
    if string == "hp":
        return PokemonStatType.HP
    elif string == "attack":
        return PokemonStatType.ATTACK
    elif string == "defense":
        return PokemonStatType.DEFENSE
    elif string == "special-attack":
        return PokemonStatType.SP_ATTACK
    elif string == "special-defense":
        return PokemonStatType.SP_DEFENSE
    elif string == "speed":
        return PokemonStatType.SPEED
    else:
        return PokemonStatType.NONE


@ustruct()
class MoveLearnData(StructBase):
    LevelLearnedAt = uproperty(int)
    MoveLearnMethod = uproperty(PokemonMoveLearnMethod)
    Move = uproperty(PrimaryDataAsset)


@ustruct()
class MoveStatChangeData(StructBase):
    Change = uproperty(int)
    Stat = uproperty(PokemonStatType)


@ustruct()
class AbilitySlotData(StructBase):
    Ability = uproperty(PrimaryDataAsset)
    Slot = uproperty(int)
    IsHidden = uproperty(bool)

@ustruct()
class EvolutionDetails(StructBase):
    Item = uproperty(PrimaryDataAsset)
    Trigger = uproperty(EvolutionTrigger)
    Gender = uproperty(int)
    HeldItem = uproperty(PrimaryDataAsset)
    KnownMove = uproperty(PrimaryDataAsset)
    KnownMoveType = uproperty(PokemonType)
    Location = uproperty(str)
    MinLevel = uproperty(int)
    MinHappiness = uproperty(int)
    MinBeauty = uproperty(int)
    MinAffection = uproperty(int)
    NeedsRain = uproperty(bool)
    PartySpecies = uproperty(int)
    PartyType = uproperty(PokemonType)
    RelativePhysicalStats = uproperty(int)
    TimeOfDay = uproperty(str)
    TradeSpecies = uproperty(int)
    TurnUpsideDown = uproperty(bool)
    Entry = uproperty(int)


class AssetType(Enum):
    Pokemon = 1
    Ability = 2
    Move = 3
    Item = 4
    EvolutionChain = 5


class ParserPokemon():

    def __init__(self):
        self.ability_assets = {}
        self.pokemon_assets = {}
        self.move_assets = {}
        self.item_assets = {}
        self.evolution_chain_assets = {}
        self.setup_database()

    def setup_database(self):
        self.ability_assets = self.load_folder_assets(ability_folder_path, AssetType.Ability)
        self.pokemon_assets = self.load_folder_assets(pokemon_folder_path, AssetType.Pokemon)
        self.move_assets = self.load_folder_assets(move_folder_path, AssetType.Move)
        self.item_assets = self.load_folder_assets(item_folder_path, AssetType.Item)
        self.evolution_chain_assets = self.load_folder_assets(evolution_chain_folder_path, AssetType.EvolutionChain)

    def get_asset_name_for_entry(self, entry: int, asset_type: AssetType) -> str:
        if asset_type == AssetType.Ability:
            obj = pb.ability(entry)
        elif asset_type == AssetType.Move:
            obj = pb.move(entry)
        elif asset_type == AssetType.Pokemon:
            obj = pb.pokemon(entry)
        elif asset_type == AssetType.EvolutionChain:
            obj = pb.evolution_chain(entry)
        else:
            return ""
        return f"{entry:04}_{obj.name}"
        
    def get_asset_from_database(self, entry: int, asset_type: AssetType) -> Object:
        if asset_type == AssetType.Ability and entry in self.ability_assets:
            return self.ability_assets[entry]
        elif asset_type == AssetType.Move and entry in self.move_assets:
            return self.move_assets[entry]
        elif asset_type == AssetType.Pokemon and entry in self.pokemon_assets:
            return self.pokemon_assets[entry]
        elif asset_type == AssetType.Item and entry in self.item_assets:
            return self.item_assets[entry]
        elif asset_type == AssetType.EvolutionChain and entry in self.evolution_chain_assets:
            return self.evolution_chain_assets[entry]
        else:
            return None

    def add_asset_to_database(self, entry, asset_data, asset_type: AssetType):
        if asset_type == AssetType.Ability:
            self.ability_assets.update({entry: asset_data})
        elif asset_type == AssetType.Move:
            self.move_assets.update({entry: asset_data})
        elif asset_type == AssetType.Pokemon:
            self.pokemon_assets.update({entry: asset_data})
        elif asset_type == AssetType.Item:
            self.item_assets.update({entry: asset_data})
        elif asset_type == AssetType.EvolutionChain:
            self.evolution_chain_assets.update({entry: asset_data})

    def get_bp_class(self, asset_type: AssetType):
        if asset_type == AssetType.Pokemon:
            return pokemon_data_bp_class
        if asset_type == AssetType.Ability:
            return ability_data_bp_class
        if asset_type == AssetType.Move:
            return move_data_bp_class
        if asset_type == AssetType.Item:
            return item_data_bp_class
        if asset_type == AssetType.EvolutionChain:
            return evolution_chain_bp_class
    
    def get_or_create_asset(self, entry: int, folder_path, asset_type: AssetType):
        asset = self.get_asset_from_database(entry, asset_type)
        if asset == None:
            print(f"Asset {entry} not found, creating it.")
            asset = self.create_asset(entry, folder_path, asset_type)
        elif asset.get_editor_property("isValid") == False:
            print(f"Asset {entry} invalid, re-creating it.")
            asset = self.create_asset(entry, folder_path, asset_type, asset)
        else:
            print(f"Asset {entry} found, skipping it.")
        
        return asset
    
    def create_asset(self, entry: int, folder_path: str, asset_type: AssetType, reuse_asset: Object = None) -> Object:
        asset = Object
        if asset_type == AssetType.Move:
            asset = self.parse_move(entry, folder_path, reuse_asset)
        elif asset_type == AssetType.Pokemon:
            asset = self.parse_pokemon(entry, folder_path, reuse_asset)
        elif asset_type == AssetType.Ability:
            asset = self.parse_ability(entry, folder_path, reuse_asset)
        elif asset_type == AssetType.Item:
            asset = self.parse_item(entry, folder_path, reuse_asset)
        elif asset_type == AssetType.EvolutionChain:
            asset = self.parse_evolution_chain(entry, folder_path, reuse_asset)
        return asset
    
    def parse_evolution_chain(self, entry: int, folder_path: str, reuse_asset = None):
        evolution_chain = pb.evolution_chain(entry, lazy_load=True)

        if evolution_chain == None:
            return

        self.parse_evolution_node(entry, folder_path, evolution_chain.chain.species, evolution_chain.chain.evolves_to, reuse_asset, evolution_chain.chain.is_baby, evolution_chain.baby_trigger_item)
    
    def parse_evolution_node(self, entry: int, folder_path: str, evolves_from, evolves_to: list = [], reuse_asset = None, is_baby = False, baby_trigger_item = None):
        if len(evolves_to) <= 0:
            return

        if reuse_asset == None:
            evolution_chain_asset = self.get_asset_from_database(evolves_from.id, AssetType.EvolutionChain)
            if evolution_chain_asset == None:
                asset_name = f"{evolves_from.id:04}_{evolves_from.name}_evolutions"
                evolution_chain_asset = AssetToolsHelpers.get_asset_tools().create_asset(asset_name, folder_path, self.get_bp_class(AssetType.EvolutionChain), factory=DataAssetFactory())
                self.add_asset_to_database(evolves_from.id, evolution_chain_asset, AssetType.EvolutionChain)
        else:
            evolution_chain_asset = reuse_asset

        evolution_chain_asset.set_editor_property("IsBaby", is_baby)        
        if baby_trigger_item != None:
            evolution_chain_asset.set_editor_property("BabyTriggerItem", self.get_or_create_asset(baby_trigger_item.id, item_folder_path, AssetType.Item))

        evolution_details_array = Array(EvolutionDetails)
        for evolution in evolves_to:
            for evolution_details in evolution.evolution_details:
                evolution_details_row = EvolutionDetails()

                evolution_details_row.Trigger = EvolutionTrigger.cast(evolution_details.trigger.id)
                if evolution_details.item != None:
                    evolution_details_row.Item = self.get_or_create_asset(evolution_details.item.id, item_folder_path, AssetType.Item)
                if evolution_details.gender != None:
                    evolution_details_row.Gender = evolution_details.gender
                if evolution_details.held_item != None:
                    evolution_details_row.HeldItem = self.get_or_create_asset(evolution_details.held_item.id, item_folder_path, AssetType.Item)
                if evolution_details.known_move != None:
                    evolution_details_row.KnownMove = self.get_or_create_asset(evolution_details.known_move.id, move_folder_path, AssetType.Move)
                if evolution_details.known_move_type != None:
                    evolution_details_row.KnownMoveType = PokemonType.cast(evolution_details.known_move_type.id)
                if evolution_details.location != None:
                    evolution_details_row.Location = evolution_details.location.name
                if evolution_details.min_level != None:
                    evolution_details_row.MinLevel = evolution_details.min_level
                if evolution_details.min_happiness != None:
                    evolution_details_row.MinHappiness = evolution_details.min_happiness
                if evolution_details.min_beauty != None:
                    evolution_details_row.MinBeauty = evolution_details.min_beauty
                if evolution_details.min_affection != None:
                    evolution_details_row.MinAffection = evolution_details.min_affection
                if evolution_details.party_species != None:
                    evolution_details_row.PartySpecies = evolution_details.party_species.id
                if evolution_details.party_type != None:
                    evolution_details_row.PartyType = PokemonType.cast(evolution_details.party_type.id)
                if evolution_details.relative_physical_stats != None:
                    evolution_details_row.RelativePhysicalStats = evolution_details.relative_physical_stats
                evolution_details_row.TimeOfDay = evolution_details.time_of_day
                if evolution_details.trade_species != None:
                    evolution_details_row.TradeSpecies = evolution_details.trade_species.id
                evolution_details_row.TurnUpsideDown = evolution_details.turn_upside_down

                evolution_details_row.NeedsRain = evolution_details.needs_overworld_rain
                evolution_details_row.Entry = evolution.species.id
                
                evolution_details_array.append(evolution_details_row)

                self.parse_evolution_node(entry, folder_path, evolution.species, evolution.evolves_to)

        evolution_chain_asset.set_editor_property("Evolutions", evolution_details_array)

        evolution_chain_asset.set_editor_property("id", evolves_from.id)
        evolution_chain_asset.set_editor_property("isValid", True)

        return evolution_chain_asset


    def parse_item(self, entry: int, folder_path: str, reuse_asset = None):
        item = pb.item(entry, lazy_load=True)

        if item.name == None:
            print(f"Item {entry} does not exits in api")
            return
        
        print(item.name)

        asset_name = f"{entry:04}_{item.name}"
        asset_name = asset_name.replace("(", "")
        asset_name = asset_name.replace(")", "")
        asset_name = asset_name.replace("&", "-")
        if reuse_asset == None:
            asset = AssetToolsHelpers.get_asset_tools().create_asset(asset_name, folder_path, self.get_bp_class(AssetType.Item), factory=DataAssetFactory())
            self.add_asset_to_database(entry, asset, AssetType.Item)
        else:
            asset = reuse_asset
        
        asset.set_editor_property("id", item.id)
        name = item.name
        name.replace("(", "")
        name.replace(")", "")
        asset.set_editor_property("name", name)
        asset.set_editor_property("cost", item.cost)
        asset.set_editor_property("flingPower", item.fling_power if item.fling_power != None else 0)
        asset.set_editor_property("flingEffect", ItemFlingEffect.cast(item.fling_effect.id if item.fling_effect != None else 0))

        attributes = Array(ItemAttribute)
        for attribute in item.attributes:
            attributes.append(ItemAttribute.cast(attribute.id))

        asset.set_editor_property("attributes", attributes)
        
        asset.set_editor_property("category", ItemCategory.cast(item.category.id))
        asset.set_editor_property("pocket", ItemPocket.cast(item.category.pocket.id))
        res = list(filter(lambda x: x.language.name == "en", item.effect_entries))
        if len(res) > 0:
            asset.set_editor_property("description", res[0].short_effect)
        asset.set_editor_property("isValid", True)

        return asset

    
    def parse_ability(self, entry: int, folder_path: str, reuse_asset = None):
        ability = pb.ability(entry, lazy_load=True)
        
        print(ability.name)

        if reuse_asset == None:
            asset_name = f"{entry:04}_{ability.name}"
            ability_asset = AssetToolsHelpers.get_asset_tools().create_asset(asset_name, folder_path, self.get_bp_class(AssetType.Ability), factory=DataAssetFactory())
            self.add_asset_to_database(entry, ability_asset, AssetType.Ability)
        else:
            ability_asset = reuse_asset
        
        ability_asset.set_editor_property("id", ability.id)
        ability_asset.set_editor_property("name", ability.name)
        res = list(filter(lambda x: x.language.name == "en", ability.effect_entries))
        if len(res) > 0:
            ability_asset.set_editor_property("description", res[0].short_effect)
        ability_asset.set_editor_property("isValid", True)

        return ability_asset


    def parse_move(self, entry: int, folder_path: str, reuse_asset = None):
        move_api = pb.move(entry, lazy_load=True)

        print(move_api.name)

        if reuse_asset == None:
            asset_name = f"{entry:04}_{move_api.name}"
            move_asset = AssetToolsHelpers.get_asset_tools().create_asset(asset_name, folder_path, self.get_bp_class(AssetType.Move), factory=DataAssetFactory())
            self.add_asset_to_database(entry, move_asset, AssetType.Move)
        else:
            move_asset = reuse_asset
   
        move_asset.set_editor_property("id", move_api.id)
        move_asset.set_editor_property("name", move_api.name)
        description = list(filter(lambda x: x.language.name == "en", move_api.effect_entries))
        if len(description) > 0:
            move_asset.set_editor_property("description", description[0].short_effect)
        move_asset.set_editor_property("accuracy", move_api.accuracy if move_api.accuracy != None else 0)
        move_asset.set_editor_property("effectChance", move_api.effect_chance if move_api.effect_chance != None else 0)
        move_asset.set_editor_property("pp", move_api.pp)
        move_asset.set_editor_property("priority", move_api.priority)
        move_asset.set_editor_property("power", move_api.power if move_api.power != None else 0)
        move_asset.set_editor_property("damageClass", PokemonMoveDamageClass.cast(move_api.damage_class.id))
        move_asset.set_editor_property("target", PokemonMoveTarget.cast(move_api.target.id))
        move_asset.set_editor_property("type", PokemonType.cast(move_api.type.id))

        meta_api = move_api.meta
        if meta_api is not None:
            move_asset.set_editor_property("Ailment", PokemonMoveAilment.cast(meta_api.ailment.id if meta_api.ailment != None and meta_api.ailment.id >= 0 else 0))
            move_asset.set_editor_property("Category", PokemonMoveCategory.cast(meta_api.category.id if meta_api.category != None else 0))
            move_asset.set_editor_property("MinHits", meta_api.min_hits if meta_api.min_hits != None else 0)
            move_asset.set_editor_property("MaxHits", meta_api.max_hits if meta_api.max_hits != None else 0)
            move_asset.set_editor_property("MinTurns", meta_api.min_turns if meta_api.min_turns != None else 0)
            move_asset.set_editor_property("MaxTurns", meta_api.max_turns if meta_api.max_turns != None else 0)
            move_asset.set_editor_property("Drain", meta_api.drain if meta_api.drain != None else 0)
            move_asset.set_editor_property("Healing", meta_api.healing if meta_api.healing != None else 0)
            move_asset.set_editor_property("CritRate", meta_api.crit_rate if meta_api.crit_rate != None else 0)
            move_asset.set_editor_property("AilmentChance", meta_api.ailment_chance if meta_api.ailment_chance != None else 0)
            move_asset.set_editor_property("FlinchChance", meta_api.flinch_chance if meta_api.flinch_chance != None else 0)
            move_asset.set_editor_property("StatChance", meta_api.stat_chance if meta_api.stat_chance != None else 0)

        stat_changes = Array(MoveStatChangeData)
        stat_changes_api = move_api.stat_changes
        for stat_change_api in stat_changes_api:
            stat_change = MoveStatChangeData()
            stat_change.Stat = map_string_stat_type_method(stat_change_api.stat.name)
            stat_change.Change = stat_change_api.change
            stat_changes.append(stat_change)
        move_asset.set_editor_property("statChanges", stat_changes)
        move_asset.set_editor_property("isValid", True)
        return move_asset
        

    def parse_pokemon(self, entry: int, folder_path: str, reuse_asset = None):
        pokemon = pb.pokemon(entry, lazy_load=True)
            
        if pokemon.name == None:
            print(f"Pokemon ID: {entry} not found. Skipping it...")
            return
        
        print(pokemon.name)

        if reuse_asset == None:
            asset_name = f"{pokemon.species.id:04}_{pokemon.name}"
            pokemon_asset = AssetToolsHelpers.get_asset_tools().create_asset(asset_name, folder_path, self.get_bp_class(AssetType.Pokemon), factory=DataAssetFactory())
            self.add_asset_to_database(entry, pokemon_asset, AssetType.Pokemon)
        else:
            pokemon_asset = reuse_asset

        pokemon_asset.set_editor_property("id", pokemon.species.id)
        pokemon_asset.set_editor_property("name", pokemon.name)
        pokemon_asset.set_editor_property("height", pokemon.height)
        pokemon_asset.set_editor_property("weight", pokemon.weight)
        pokemon_asset.set_editor_property("baseExperience", pokemon.base_experience)
        pokemon_asset.set_editor_property("genderRate", pokemon.species.gender_rate)
        pokemon_asset.set_editor_property("captureRate", pokemon.species.capture_rate)
        pokemon_asset.set_editor_property("hasGenderDifferences", pokemon.species.has_gender_differences)
        pokemon_asset.set_editor_property("isBaby", pokemon.species.is_baby)
        pokemon_asset.set_editor_property("isLegendary", pokemon.species.is_legendary)
        pokemon_asset.set_editor_property("isMythical", pokemon.species.is_mythical)
        pokemon_asset.set_editor_property("growthRate", GrowthRateType.cast(pokemon.species.growth_rate.id))

        if len(pokemon.forms) != 0:
            form = pokemon.forms[0]

            form_id = 10 + int(form.form_order) if form.form_name != "" else 0
            form_regional = map_regional_forms(form.form_name)
            if form_regional != PokemonRegionalFormType.NONE:
                form_id = form.form_order - 1

            pokemon_asset.set_editor_property("isDefaultForm",  form.is_default)
            pokemon_asset.set_editor_property("form",  form_id)
            pokemon_asset.set_editor_property("regionalForm",  form_regional)
            pokemon_asset.set_editor_property("isMega", form.is_mega)

        pokedex_descriptions = list(filter(lambda flavor_text_entries: flavor_text_entries["version"]["name"] == "scarlet-violet" and flavor_text_entries["language"]["name"] == "en", pokemon.species.flavor_text_entries))
        if len(pokedex_descriptions) == 0:
            pokedex_descriptions = list(filter(lambda flavor_text_entries: flavor_text_entries["version"]["name"] == "ultra-sun-ultra-moon" and flavor_text_entries["language"]["name"] == "en", pokemon.species.flavor_text_entries))
        if len(pokedex_descriptions) == 0:
            pokedex_descriptions = list(filter(lambda flavor_text_entries: flavor_text_entries["version"]["name"] == "sword" and flavor_text_entries["language"]["name"] == "en", pokemon.species.flavor_text_entries))
        if len(pokedex_descriptions) == 0:
            pokedex_descriptions = list(filter(lambda flavor_text_entries: flavor_text_entries["version"]["name"] == "shield" and flavor_text_entries["language"]["name"] == "en", pokemon.species.flavor_text_entries))
        if len(pokedex_descriptions) == 0:
            pokedex_descriptions = list(filter(lambda flavor_text_entries: flavor_text_entries["version"]["name"] == "black-2" and flavor_text_entries["language"]["name"] == "en", pokemon.species.flavor_text_entries))
        if len(pokedex_descriptions) == 0:
            pokedex_descriptions = list(filter(lambda flavor_text_entries: flavor_text_entries["version"]["name"] == "white-2" and flavor_text_entries["language"]["name"] == "en", pokemon.species.flavor_text_entries))

        if len(pokedex_descriptions) != 0:
            pokemon_asset.set_editor_property("pokedexDescription", pokedex_descriptions[0]["flavor_text"])
        
        pokemon_genera = list(filter(lambda genera: genera["language"]["name"] == "en", pokemon.species.genera))
        pokemon_asset.set_editor_property("genera", pokemon_genera[0]["genus"])
        
        pokemon_asset.set_editor_property("evolutionChain", self.get_asset_from_database(pokemon.id, AssetType.EvolutionChain))

        types = [PokemonType.NONE, PokemonType.NONE]
        for type in pokemon.types:
            types[type.slot-1] = PokemonType.cast(type.type.id)
        pokemon_asset.set_editor_property("types", types)
        
        stats = {}
        evs = {}
        for stat in range(6):
            pokemon_stat_type = PokemonStatType.cast(stat+1)
            stats.update({pokemon_stat_type: pokemon.stats[stat].base_stat})
            evs.update({pokemon_stat_type: pokemon.stats[stat].effort})

        pokemon_asset.set_editor_property("stats", stats)
        pokemon_asset.set_editor_property("evs", evs)

        abilities = Array(AbilitySlotData)
        for ability in pokemon.abilities:
            ability_slot_data = AbilitySlotData()
            ability_slot_data.Ability = self.get_or_create_asset(ability.ability.id, ability_folder_path, AssetType.Ability)
            ability_slot_data.IsHidden = ability.is_hidden
            ability_slot_data.Slot = ability.slot
            abilities.append(ability_slot_data)

        pokemon_asset.set_editor_property("abilities", abilities)

        move_learns = Array(MoveLearnData)
        for move in pokemon.moves:
            version_group_details = list(filter(lambda version: version.version_group.name == "scarlet-violet", move.version_group_details))
            if len(version_group_details) == 0:
                version_group_details = list(filter(lambda version: version.version_group.name == "ultra-sun-ultra-moon", move.version_group_details))

            for version in version_group_details:
                move_learn_asset = MoveLearnData()
                move_learn_asset.LevelLearnedAt = version.level_learned_at
                move_learn_asset.MoveLearnMethod = PokemonMoveLearnMethod.cast(map_string_to_move_learn_method(version.move_learn_method.name))
                move_learn_asset.Move = self.get_or_create_asset(move.move.id, move_folder_path, AssetType.Move)
                move_learns.append(move_learn_asset)

        pokemon_asset.set_editor_property("moves", move_learns)

        # skeletal_mesh_path = f"/Game/Models/pm{entry:04}_00_00/pm{entry:04}_00_00.pm{entry:04}_00_00"
        # skeletal_mesh = load_asset(skeletal_mesh_path)
        # pokemon_asset.set_editor_property("skeletalMesh", skeletal_mesh)

        pokemon_asset.set_editor_property("isValid", True)
        return pokemon_asset


    def load_folder_assets(self, folder_path, asset_type: AssetType):
        loaded_assets = {}
        assets = AssetRegistryHelpers.get_asset_registry().get_assets_by_path(folder_path)
        print(f"Found {len(assets)} assets in {folder_path}")
        for asset in assets:
            full_name = asset.get_full_name()
            path = full_name.split(' ')[-1]
            loaded_asset = load_asset(path)
            property_name = "id" if asset_type != AssetType.Pokemon else "internalId"
            loaded_asset.set_editor_property("id", int(loaded_asset.get_name().split("_")[0][-4:]))
            entry = loaded_asset.get_editor_property(property_name)
            loaded_assets.update({entry: loaded_asset})

        return loaded_assets



    def start_parsing(self, initial_entry, final_entry):
        for i in range (initial_entry, final_entry):
            self.get_or_create_asset(i, pokemon_folder_path, AssetType.Pokemon)



    def rename_to_new_format(self, asset: Object):
        if asset is None:
            return
        
        id = asset.get_editor_property("id")
        name = asset.get_editor_property("name")
        new_name = f"{id:04}_{name}"
        if asset.get_name() != new_name:
            p = Path(asset.get_path_name())
            EditorAssetLibrary.rename_loaded_asset(asset, f"{p.parent}_Fixed/{new_name}")


if __name__=="__main__":
    # with Pool() as pool:
    #     result = pool.map(parse_move, range(5))

    # with cProfile.Profile() as pr:
    #     pr.enable()
        cache.set_cache(cache_path)
        parser = ParserPokemon()
        parser.start_parsing(10000, 10450)
        # for i in range (1, 550):
        #     parser.parse_evolution_chain(i, evolution_chain_folder_path)
        # for i in range (1, 500):
        #     parser.rename_to_new_format(parser.get_asset_from_database(i, AssetType.Pokemon))

        # pr.disable()
        # pr.print_stats(sort="cumtime")
