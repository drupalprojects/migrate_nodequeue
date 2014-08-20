<?php

/**
 * Migrate nodequeue-node relationships.
 * TODO remove any refs
 */
class NodequeueNodeDrupal6Migration extends DrupalMigration {
  protected $source_qids;

  public function __construct($args = array()) {
    $args['description'] = 'Migration of nodequeue content from Drupal 6';
    parent::__construct($args);
    $this->source_qids = empty($args['source_qids']) ? array() : $args['source_qids'];

    // Declare source and destination objects.
    $options = array('fix_field_names' => array(), 'map_joinable' => FALSE);
    $this->source      = new MigrateDrupal6SourceSql($this->query(), array(), NULL, $options);
    $this->destination = new NodequeueNodeDestination();

    // Map source with destination. Assume table schema is the same in D6 and D7.
    $key_schema = $this->destination->getKeySchema();
    $this->map = new MigrateSQLMap($this->machineName, $key_schema, $key_schema);

    // Declare field mappings.
    isset($args['migration_queues'])
      ? $this->addFieldMapping('qid', 'qid')->sourceMigration($args['migration_queues'])
      : $this->addFieldMapping('qid', 'qid');
    isset($args['migration_subqueues'])
      ? $this->addFieldMapping('sqid', 'sqid')->sourceMigration($args['migration_subqueues'])
      : $this->addFieldMapping('sqid', 'sqid');
    isset($args['migrations_nodes'])
      ? $this->addFieldMapping('nid', 'nid')->sourceMigration($args['migrations_nodes'])
      : $this->addFieldMapping('nid', 'nid');
    $this->addSimpleMappings(array('position', 'timestamp'));
  }

  /**
   * @override
   *
   * Get {nodequeue_node} data.
   * Join with {nodequeue_queue} to have the machine name.
   */
  protected function query() {
    $query = Database::getConnection('default', 'd6')
      ->select('nodequeue_nodes', 'n')
      ->fields('n', array('qid', 'sqid', 'nid', 'position', 'timestamp'));
    if (!empty($this->source_qids)) {
      $query = $query->condition('qid', $this->source_qids, 'IN');
    }
    return $query;
  }

  /**
   * @override
   */
  public function prepareRow($row) {
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }
    // TODO Make sure the D7 queue exists
  }

}
