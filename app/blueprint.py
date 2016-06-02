from flask import Blueprint
import views

blueprint = Blueprint('lightshow', __name__)
blueprint.add_url_rule('/', view_func=views.IndexView.as_view('Home'))
blueprint.add_url_rule('/index', view_func=views.IndexView.as_view('Index'))
blueprint.add_url_rule('/playShow', view_func=views.PlayView.as_view('Play'))
blueprint.add_url_rule('/grid', view_func=views.GridView.as_view('Grid'))
blueprint.add_url_rule('/getScripts', view_func=views.ScriptsView.as_view("GetScripts"))
blueprint.add_url_rule('/layout', view_func=views.GridLayoutView.as_view("GridLayout"))
blueprint.add_url_rule('/playScript', view_func=views.PlayScriptView.as_view("PlayScript"))
