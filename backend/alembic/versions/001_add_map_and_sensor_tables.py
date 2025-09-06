"""Add map and sensor tables

Revision ID: 001
Revises: 
Create Date: 2025-01-28 14:47:00.000000

"""
from alembic import op
import sqlalchemy as sa
from sqlalchemy.dialects import sqlite

# revision identifiers, used by Alembic.
revision = '001'
down_revision = None
branch_labels = None
depends_on = None


def upgrade():
    # Create maps table
    op.create_table('maps',
        sa.Column('id', sa.Integer(), nullable=False),
        sa.Column('map_id', sa.String(length=50), nullable=False),
        sa.Column('name', sa.String(length=100), nullable=False),
        sa.Column('resolution', sa.Float(), nullable=False),
        sa.Column('width', sa.Integer(), nullable=False),
        sa.Column('height', sa.Integer(), nullable=False),
        sa.Column('occupancy_grid', sa.JSON(), nullable=False),
        sa.Column('robot_trajectory', sa.JSON(), nullable=True),
        sa.Column('rfid_positions', sa.JSON(), nullable=True),
        sa.Column('metadata', sa.JSON(), nullable=True),
        sa.Column('is_active', sa.Boolean(), nullable=True),
        sa.Column('created_at', sa.DateTime(timezone=True), server_default=sa.text('(CURRENT_TIMESTAMP)'), nullable=True),
        sa.Column('updated_at', sa.DateTime(timezone=True), server_default=sa.text('(CURRENT_TIMESTAMP)'), nullable=True),
        sa.PrimaryKeyConstraint('id')
    )
    op.create_index(op.f('ix_maps_id'), 'maps', ['id'], unique=False)
    op.create_index(op.f('ix_maps_map_id'), 'maps', ['map_id'], unique=True)

    # Create map_sessions table
    op.create_table('map_sessions',
        sa.Column('id', sa.Integer(), nullable=False),
        sa.Column('session_id', sa.String(length=50), nullable=False),
        sa.Column('map_id', sa.String(length=50), nullable=False),
        sa.Column('start_time', sa.DateTime(timezone=True), server_default=sa.text('(CURRENT_TIMESTAMP)'), nullable=True),
        sa.Column('end_time', sa.DateTime(timezone=True), nullable=True),
        sa.Column('is_active', sa.Boolean(), nullable=True),
        sa.Column('total_scans', sa.Integer(), nullable=True),
        sa.Column('mapping_quality', sa.Float(), nullable=True),
        sa.Column('created_at', sa.DateTime(timezone=True), server_default=sa.text('(CURRENT_TIMESTAMP)'), nullable=True),
        sa.ForeignKeyConstraint(['map_id'], ['maps.map_id'], ),
        sa.PrimaryKeyConstraint('id')
    )
    op.create_index(op.f('ix_map_sessions_id'), 'map_sessions', ['id'], unique=False)
    op.create_index(op.f('ix_map_sessions_session_id'), 'map_sessions', ['session_id'], unique=True)

    # Create robot_positions table
    op.create_table('robot_positions',
        sa.Column('id', sa.Integer(), nullable=False),
        sa.Column('map_id', sa.String(length=50), nullable=False),
        sa.Column('session_id', sa.String(length=50), nullable=True),
        sa.Column('x', sa.Float(), nullable=False),
        sa.Column('y', sa.Float(), nullable=False),
        sa.Column('theta', sa.Float(), nullable=False),
        sa.Column('confidence', sa.Float(), nullable=False),
        sa.Column('source', sa.String(length=20), nullable=False),
        sa.Column('timestamp', sa.DateTime(timezone=True), server_default=sa.text('(CURRENT_TIMESTAMP)'), nullable=True),
        sa.ForeignKeyConstraint(['map_id'], ['maps.map_id'], ),
        sa.ForeignKeyConstraint(['session_id'], ['map_sessions.session_id'], ),
        sa.PrimaryKeyConstraint('id')
    )
    op.create_index(op.f('ix_robot_positions_id'), 'robot_positions', ['id'], unique=False)

    # Create rfid_tags table
    op.create_table('rfid_tags',
        sa.Column('id', sa.Integer(), nullable=False),
        sa.Column('rfid_id', sa.String(length=50), nullable=False),
        sa.Column('position_x', sa.Float(), nullable=False),
        sa.Column('position_y', sa.Float(), nullable=False),
        sa.Column('orientation', sa.Float(), nullable=True),
        sa.Column('description', sa.Text(), nullable=True),
        sa.Column('is_active', sa.Boolean(), nullable=True),
        sa.Column('created_at', sa.DateTime(timezone=True), server_default=sa.text('(CURRENT_TIMESTAMP)'), nullable=True),
        sa.Column('updated_at', sa.DateTime(timezone=True), server_default=sa.text('(CURRENT_TIMESTAMP)'), nullable=True),
        sa.PrimaryKeyConstraint('id')
    )
    op.create_index(op.f('ix_rfid_tags_id'), 'rfid_tags', ['id'], unique=False)
    op.create_index(op.f('ix_rfid_tags_rfid_id'), 'rfid_tags', ['rfid_id'], unique=True)

    # Create sensor_data table
    op.create_table('sensor_data',
        sa.Column('id', sa.Integer(), nullable=False),
        sa.Column('sensor_type', sa.String(length=20), nullable=False),
        sa.Column('sensor_id', sa.String(length=50), nullable=False),
        sa.Column('data', sa.JSON(), nullable=False),
        sa.Column('timestamp', sa.DateTime(timezone=True), server_default=sa.text('(CURRENT_TIMESTAMP)'), nullable=True),
        sa.Column('quality', sa.Float(), nullable=True),
        sa.Column('is_valid', sa.Boolean(), nullable=True),
        sa.PrimaryKeyConstraint('id')
    )
    op.create_index(op.f('ix_sensor_data_id'), 'sensor_data', ['id'], unique=False)

    # Create localization_configs table
    op.create_table('localization_configs',
        sa.Column('id', sa.Integer(), nullable=False),
        sa.Column('config_key', sa.String(length=100), nullable=False),
        sa.Column('config_value', sa.JSON(), nullable=False),
        sa.Column('config_type', sa.String(length=20), nullable=False),
        sa.Column('description', sa.Text(), nullable=True),
        sa.Column('is_active', sa.Boolean(), nullable=True),
        sa.Column('created_at', sa.DateTime(timezone=True), server_default=sa.text('(CURRENT_TIMESTAMP)'), nullable=True),
        sa.Column('updated_at', sa.DateTime(timezone=True), server_default=sa.text('(CURRENT_TIMESTAMP)'), nullable=True),
        sa.Column('updated_by', sa.Integer(), nullable=True),
        sa.PrimaryKeyConstraint('id')
    )
    op.create_index(op.f('ix_localization_configs_id'), 'localization_configs', ['id'], unique=False)
    op.create_index(op.f('ix_localization_configs_config_key'), 'localization_configs', ['config_key'], unique=True)

    # Create sensor_configurations table
    op.create_table('sensor_configurations',
        sa.Column('id', sa.Integer(), nullable=False),
        sa.Column('sensor_id', sa.String(length=50), nullable=False),
        sa.Column('sensor_type', sa.String(length=20), nullable=False),
        sa.Column('name', sa.String(length=100), nullable=False),
        sa.Column('description', sa.Text(), nullable=True),
        sa.Column('configuration', sa.JSON(), nullable=False),
        sa.Column('calibration_data', sa.JSON(), nullable=True),
        sa.Column('is_active', sa.Boolean(), nullable=True),
        sa.Column('last_calibration', sa.DateTime(timezone=True), nullable=True),
        sa.Column('created_at', sa.DateTime(timezone=True), server_default=sa.text('(CURRENT_TIMESTAMP)'), nullable=True),
        sa.Column('updated_at', sa.DateTime(timezone=True), server_default=sa.text('(CURRENT_TIMESTAMP)'), nullable=True),
        sa.PrimaryKeyConstraint('id')
    )
    op.create_index(op.f('ix_sensor_configurations_id'), 'sensor_configurations', ['id'], unique=False)
    op.create_index(op.f('ix_sensor_configurations_sensor_id'), 'sensor_configurations', ['sensor_id'], unique=True)

    # Create sensor_readings table
    op.create_table('sensor_readings',
        sa.Column('id', sa.Integer(), nullable=False),
        sa.Column('sensor_id', sa.String(length=50), nullable=False),
        sa.Column('sensor_type', sa.String(length=20), nullable=False),
        sa.Column('reading_data', sa.JSON(), nullable=False),
        sa.Column('processed_data', sa.JSON(), nullable=True),
        sa.Column('quality_score', sa.Float(), nullable=True),
        sa.Column('is_valid', sa.Boolean(), nullable=True),
        sa.Column('error_code', sa.String(length=20), nullable=True),
        sa.Column('timestamp', sa.DateTime(timezone=True), server_default=sa.text('(CURRENT_TIMESTAMP)'), nullable=True),
        sa.PrimaryKeyConstraint('id')
    )
    op.create_index(op.f('ix_sensor_readings_id'), 'sensor_readings', ['id'], unique=False)

    # Create sensor_status table
    op.create_table('sensor_status',
        sa.Column('id', sa.Integer(), nullable=False),
        sa.Column('sensor_id', sa.String(length=50), nullable=False),
        sa.Column('sensor_type', sa.String(length=20), nullable=False),
        sa.Column('status', sa.String(length=20), nullable=False),
        sa.Column('last_reading_time', sa.DateTime(timezone=True), nullable=True),
        sa.Column('error_count', sa.Integer(), nullable=True),
        sa.Column('last_error', sa.Text(), nullable=True),
        sa.Column('health_score', sa.Float(), nullable=True),
        sa.Column('updated_at', sa.DateTime(timezone=True), server_default=sa.text('(CURRENT_TIMESTAMP)'), nullable=True),
        sa.PrimaryKeyConstraint('id')
    )
    op.create_index(op.f('ix_sensor_status_id'), 'sensor_status', ['id'], unique=False)
    op.create_index(op.f('ix_sensor_status_sensor_id'), 'sensor_status', ['sensor_id'], unique=True)

    # Create sensor_calibrations table
    op.create_table('sensor_calibrations',
        sa.Column('id', sa.Integer(), nullable=False),
        sa.Column('sensor_id', sa.String(length=50), nullable=False),
        sa.Column('sensor_type', sa.String(length=20), nullable=False),
        sa.Column('calibration_type', sa.String(length=20), nullable=False),
        sa.Column('calibration_data', sa.JSON(), nullable=False),
        sa.Column('reference_data', sa.JSON(), nullable=True),
        sa.Column('accuracy', sa.Float(), nullable=True),
        sa.Column('is_valid', sa.Boolean(), nullable=True),
        sa.Column('performed_by', sa.Integer(), nullable=True),
        sa.Column('created_at', sa.DateTime(timezone=True), server_default=sa.text('(CURRENT_TIMESTAMP)'), nullable=True),
        sa.Column('expires_at', sa.DateTime(timezone=True), nullable=True),
        sa.PrimaryKeyConstraint('id')
    )
    op.create_index(op.f('ix_sensor_calibrations_id'), 'sensor_calibrations', ['id'], unique=False)


def downgrade():
    op.drop_index(op.f('ix_sensor_calibrations_id'), table_name='sensor_calibrations')
    op.drop_table('sensor_calibrations')
    op.drop_index(op.f('ix_sensor_status_sensor_id'), table_name='sensor_status')
    op.drop_index(op.f('ix_sensor_status_id'), table_name='sensor_status')
    op.drop_table('sensor_status')
    op.drop_index(op.f('ix_sensor_readings_id'), table_name='sensor_readings')
    op.drop_table('sensor_readings')
    op.drop_index(op.f('ix_sensor_configurations_sensor_id'), table_name='sensor_configurations')
    op.drop_index(op.f('ix_sensor_configurations_id'), table_name='sensor_configurations')
    op.drop_table('sensor_configurations')
    op.drop_index(op.f('ix_localization_configs_config_key'), table_name='localization_configs')
    op.drop_index(op.f('ix_localization_configs_id'), table_name='localization_configs')
    op.drop_table('localization_configs')
    op.drop_index(op.f('ix_sensor_data_id'), table_name='sensor_data')
    op.drop_table('sensor_data')
    op.drop_index(op.f('ix_rfid_tags_rfid_id'), table_name='rfid_tags')
    op.drop_index(op.f('ix_rfid_tags_id'), table_name='rfid_tags')
    op.drop_table('rfid_tags')
    op.drop_index(op.f('ix_robot_positions_id'), table_name='robot_positions')
    op.drop_table('robot_positions')
    op.drop_index(op.f('ix_map_sessions_session_id'), table_name='map_sessions')
    op.drop_index(op.f('ix_map_sessions_id'), table_name='map_sessions')
    op.drop_table('map_sessions')
    op.drop_index(op.f('ix_maps_map_id'), table_name='maps')
    op.drop_index(op.f('ix_maps_id'), table_name='maps')
    op.drop_table('maps')
