<?php

/**
 * Migrate nodequeues.
 */
class NodequeueQueueDrupal6Migration extends DrupalMigration {
  protected $source_qids;

  public function __construct($args) {
    parent::__construct($args);
    $this->source_qids = empty($args['source_qids']) ? array() : $args['source_qids'];

    // Declare source and destination objects.
    $options = array('fix_field_names' => array(), 'map_joinable' => FALSE);
    $this->source      = new MigrateDrupal6SourceSql($this->query(), array(), NULL, $options);
    $this->destination = new NodequeueQueueDestination();

    // Map source with destination. Schema is the same in D6 and D7.
    $key_schema = $this->destination->getKeySchema();
    $this->map = new MigrateSQLMap($this->machineName, $key_schema, $key_schema);

    // Declare field mappings.
    $this->addSimpleMappings(array('name', 'title', 'subqueue_title', 'size', 'link', 'link_remove', 'owner', 'show_in_ui', 'show_in_tab', 'show_in_links', 'reference', 'reverse', 'i18n'));
  }

  /**
   * @override
   *
   * Get {nodequeue_queue} data.
   */
  protected function query() {
    $query = Database::getConnection('default', $this->sourceConnection)
      ->select('nodequeue_queue', 'q')
      ->fields('q', array('qid', 'name', 'title', 'subqueue_title', 'size', 'link', 'link_remove', 'owner', 'show_in_ui', 'show_in_tab', 'show_in_links', 'reference', 'reverse', 'i18n'));
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
    // TODO do not migrate if queue already exists
  }

}
