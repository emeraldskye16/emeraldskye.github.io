from pymongo import MongoClient

class AnimalShelter(object):
    """ CRUD operations for Animal collection in MongoDB """

    def __init__(self, username, password):
        HOST = 'localhost'
        PORT = 27017
        DB = 'aac'
        COL = 'animals'

        # Build authenticated MongoDB client
        self.client = MongoClient(f"mongodb://{username}:{password}@{HOST}:{PORT}")
        self.database = self.client[DB]
        self.collection = self.database[COL]

    # Create
    def create(self, data):
        if data:
            result = self.collection.insert_one(data)
            return True if result.inserted_id else False
        else:
            raise Exception("Nothing to save, data parameter is empty")

    # Read
    def read(self, criteria=None):
        if criteria:
            return list(self.collection.find(criteria, {"_id": False}))
        else:
            return list(self.collection.find({}, {"_id": False}))

    # Update
    def update(self, searchData, updateData):
        if searchData:
            result = self.collection.update_many(searchData, {"$set": updateData})
            return result.modified_count
        else:
            raise Exception("searchData cannot be empty")

    # Delete
    def delete(self, deleteData):
        if deleteData:
            result = self.collection.delete_many(deleteData)
            return result.deleted_count
        else:
            raise Exception("deleteData cannot be empty")