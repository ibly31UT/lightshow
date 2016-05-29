"""empty message

Revision ID: aaeac1c10b6b
Revises: None
Create Date: 2016-05-28 18:23:46.048734

"""

# revision identifiers, used by Alembic.
revision = 'aaeac1c10b6b'
down_revision = None

from alembic import op
import sqlalchemy as sa


def upgrade():
    ### commands auto generated by Alembic - please adjust! ###
    op.create_table('scripts',
    sa.Column('id', sa.Integer(), nullable=False),
    sa.Column('name', sa.String(length=150), nullable=True),
    sa.Column('script', sa.Text(), nullable=True),
    sa.PrimaryKeyConstraint('id')
    )
    op.create_table('selector',
    sa.Column('id', sa.Integer(), nullable=False),
    sa.Column('name', sa.String(length=150), nullable=True),
    sa.Column('html', sa.Text(), nullable=True),
    sa.Column('js', sa.Text(), nullable=True),
    sa.PrimaryKeyConstraint('id')
    )
    op.create_table('script_selector',
    sa.Column('script_id', sa.Integer(), nullable=True),
    sa.Column('selector_id', sa.Integer(), nullable=True),
    sa.ForeignKeyConstraint(['script_id'], ['scripts.id'], ),
    sa.ForeignKeyConstraint(['selector_id'], ['selector.id'], )
    )
    ### end Alembic commands ###


def downgrade():
    ### commands auto generated by Alembic - please adjust! ###
    op.drop_table('script_selector')
    op.drop_table('selector')
    op.drop_table('scripts')
    ### end Alembic commands ###