from flask.ext.script import Manager
from flask.ext.migrate import Migrate, MigrateCommand
from app import app
from app import DB

migrate = Migrate(app, DB)
manager = Manager(app)
manager.add_command('db', MigrateCommand)

if __name__ == '__main__':
    manager.run()
