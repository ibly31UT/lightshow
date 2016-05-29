from extensions import DB


script_selector = DB.Table('script_selector', DB.Model.metadata,
    DB.Column('script_id', DB.Integer, DB.ForeignKey('scripts.id')),
    DB.Column('selector_id', DB.Integer, DB.ForeignKey('selector.id'))
)


class LightScript(DB.Model):
    """
    The lightscript object to store to the DB
    name : name of script to appear on grids
    script : the script stored in the DB
    color : string class name for the button, to differentiate
    color_one : flag to determine if a first color input is required
    color_two : flag to determine if a second color input is required
    speed : flag to determine if speed slider is required
    division : flag to determine if subdivision is required
    """
    __tablename__ = "scripts"
    id = DB.Column(DB.Integer, primary_key=True)
    name = DB.Column(DB.String(150))
    script = DB.Column(DB.Text)
    color = DB.Column(DB.String(50))
    selectors = DB.relationship(
        "Selector",
        secondary=script_selector)

    def __str__(self):
        return self.name


class GridLayout(DB.Model):
    """
    grid layout object to store preferences of user's grid
    user : which user ID this grid layout is associated with
    scripts : json serialized array of scripts with grid locations
    """
    __tablename__ = "gridlayouts"
    id = DB.Column(DB.Integer, primary_key=True)
    user = DB.Column(DB.Integer)
    scripts = DB.Column(DB.Text)

    def __init__(self, user, scripts):
        self.user = user
        self.scripts = scripts

    def __str__(self):
        return self.scripts


class Selector(DB.Model):
    """
    selector objects for colors and sliders
    name : name of the selector ie color one
    html: html to render the selector
    js: js needed to render the selector in the widget
    """
    __tablename__ = "selector"
    id = DB.Column(DB.Integer, primary_key=True)
    name = DB.Column(DB.String(150))
    html = DB.Column(DB.Text)
    js = DB.Column(DB.Text)

    def __str__(self):
        return self.name
