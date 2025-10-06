"""add registers table

Revision ID: 74b980b935e7
Revises: 001
Create Date: 2025-10-06 22:39:01.287470

"""
from alembic import op
import sqlalchemy as sa
from sqlalchemy.dialects import sqlite

# revision identifiers, used by Alembic.
revision = '74b980b935e7'
down_revision = '001'
branch_labels = None
depends_on = None


def upgrade() -> None:
    # ### registers table only (SQLite-safe) ###
    bind = op.get_bind()
    inspector = sa.inspect(bind)
    tables = inspector.get_table_names()
    if 'registers' not in tables:
        op.create_table('registers',
            sa.Column('id', sa.Integer(), nullable=False),
            sa.Column('name', sa.String(length=100), nullable=False),
            sa.Column('address', sa.String(length=50), nullable=False),
            sa.Column('mode', sa.Enum('READ', 'WRITE', 'READ_WRITE', name='registermode'), nullable=False),
            sa.Column('access_level', sa.Enum('USER', 'ADMIN', 'SYSTEM', name='registeraccesslevel'), nullable=False),
            sa.Column('is_safe_register', sa.Boolean(), nullable=False),
            sa.Column('unit', sa.String(length=20), nullable=True),
            sa.Column('description', sa.String(length=255), nullable=True),
            sa.Column('created_at', sa.DateTime(timezone=True), server_default=sa.text('(CURRENT_TIMESTAMP)'), nullable=True),
            sa.Column('updated_at', sa.DateTime(timezone=True), server_default=sa.text('(CURRENT_TIMESTAMP)'), nullable=True),
            sa.PrimaryKeyConstraint('id')
        )
        op.create_index(op.f('ix_registers_address'), 'registers', ['address'], unique=False)
        op.create_index(op.f('ix_registers_id'), 'registers', ['id'], unique=False)
        op.create_index(op.f('ix_registers_name'), 'registers', ['name'], unique=False)
    # ### end registers table ###


def downgrade() -> None:
    # ### registers table only rollback ###
    op.drop_index(op.f('ix_registers_name'), table_name='registers')
    op.drop_index(op.f('ix_registers_id'), table_name='registers')
    op.drop_index(op.f('ix_registers_address'), table_name='registers')
    op.drop_table('registers')
    # ### end ###
