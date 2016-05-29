from flask import Flask
from blueprint import blueprint
from flask_admin import Admin
from flask_admin.contrib.sqla import ModelView

from extensions import DB
from models import LightScript, GridLayout, Selector

app = Flask(__name__)
app.config.from_object('app.config')
DB.init_app(app)
app.register_blueprint(blueprint)

admin = Admin(app, name="Lightshow", template_mode='bootstrap3')
admin.add_view(ModelView(LightScript, DB.session))
admin.add_view(ModelView(Selector, DB.session))
admin.add_view(ModelView(GridLayout, DB.session))
