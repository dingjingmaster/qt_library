#include "qgsettings.h"

#include "qconftype.h"

#include <glib.h>
#include <gio/gio.h>

#include <QString>

struct QGSettingsPrivate
{
    QByteArray schemaId;
    QByteArray path;
    GSettings *settings;
    GSettingsSchema *schema;
    gulong signalHandlerId;

    static void settingChanged(GSettings *settings, const gchar *key, gpointer userData);
};

void QGSettingsPrivate::settingChanged(GSettings *, const gchar *key, gpointer userData)
{
    QGSettings *self = (QGSettings *)userData;

    QMetaObject::invokeMethod(self, "changed", Qt::QueuedConnection, Q_ARG(QString, qtify_name(key)));
}


QGSettings::QGSettings(const QByteArray &schemaId, const QByteArray &path, QObject *parent) : QObject(parent)
{
    mPriv = new QGSettingsPrivate;
    mPriv->schemaId = schemaId;
    mPriv->path = path;

    if (mPriv->path.isEmpty()) {
        mPriv->settings = g_settings_new(mPriv->schemaId.constData());
    } else {
        mPriv->settings = g_settings_new_with_path(mPriv->schemaId.constData(), mPriv->path.constData());
    }
    g_object_get(mPriv->settings, "settings-schema", &mPriv->schema, NULL);
    mPriv->signalHandlerId = g_signal_connect(mPriv->settings, "changed", G_CALLBACK(QGSettingsPrivate::settingChanged), this);
}

QGSettings::~QGSettings()
{
    if (mPriv->schema) {
        g_settings_sync ();
        g_signal_handler_disconnect(mPriv->settings, mPriv->signalHandlerId);
        g_object_unref (mPriv->settings);
        g_settings_schema_unref (mPriv->schema);
    }
    delete mPriv;
}

QVariant QGSettings::get(const QString &key) const
{
    gchar *gkey = unqtify_name(key);
    GVariant *value = g_settings_get_value(mPriv->settings, gkey);
    QVariant qvalue = qconf_types_to_qvariant(value);
    g_variant_unref(value);
    g_free(gkey);

    return qvalue;
}

void QGSettings::set(const QString &key, const QVariant &value)
{
    if (!trySet(key, value))
        qWarning("unable to set key '%s' to value '%s'", key.toUtf8().constData(), value.toString().toUtf8().constData());
}

bool QGSettings::trySet(const QString &key, const QVariant &value)
{
    gchar *gkey = unqtify_name(key);
    bool success = false;

    /* fetch current value to find out the exact type */
    GVariant *cur = g_settings_get_value(mPriv->settings, gkey);

    GVariant *new_value = qconf_types_collect_from_variant(g_variant_get_type (cur), value);
    if (new_value)
        success = g_settings_set_value(mPriv->settings, gkey, new_value);

    g_free(gkey);
    g_variant_unref (cur);

    return success;
}

QStringList QGSettings::keys() const
{
    QStringList list;
    gchar **keys = g_settings_list_keys(mPriv->settings);
    for (int i = 0; keys[i]; i++)
        list.append(qtify_name(keys[i]));

    g_strfreev(keys);

    return list;
}

QVariantList QGSettings::choices(const QString &qkey) const
{
    gchar *key = unqtify_name (qkey);
    GSettingsSchemaKey *schema_key = g_settings_schema_get_key (mPriv->schema, key);
    GVariant *range = g_settings_schema_key_get_range(schema_key);
    g_settings_schema_key_unref (schema_key);
    g_free(key);

    if (range == NULL)
        return QVariantList();

    const gchar *type;
    GVariant *value;
    g_variant_get(range, "(&sv)", &type, &value);

    QVariantList choices;
    if (g_str_equal(type, "enum")) {
        GVariantIter iter;
        GVariant *child;

        g_variant_iter_init (&iter, value);
        while ((child = g_variant_iter_next_value(&iter))) {
            choices.append(qconf_types_to_qvariant(child));
            g_variant_unref(child);
        }
    }

    g_variant_unref (value);
    g_variant_unref (range);

    return choices;
}

void QGSettings::reset(const QString &qkey)
{
    gchar *key = unqtify_name(qkey);
    g_settings_reset(mPriv->settings, key);
    g_free(key);
}

bool QGSettings::isSchemaInstalled(const QByteArray &schemaId)
{
    GSettingsSchemaSource *source = g_settings_schema_source_get_default ();
    GSettingsSchema *schema = g_settings_schema_source_lookup (source, schemaId.constData(), TRUE);
    if (schema) {
        g_settings_schema_unref (schema);
        return true;
    } else {
        return false;
    }
}
